#include "FrameManager.h"

namespace efvk
{
	FrameManager::FrameManager(GraphicsContext& ctx, u32 window_width, u32 window_height)
	{
		vk::Result result = vk::Result::eSuccess;

		/* Select a surface format. No special logic for now, just choose the first available */
		std::vector<vk::SurfaceFormatKHR> supported_surface_formats;
		std::tie(result, supported_surface_formats) = ctx.physical_device.getSurfaceFormatsKHR(ctx.surface);
		assert(result == vk::Result::eSuccess);
		assert(supported_surface_formats.size() > 0);
		const vk::SurfaceFormatKHR surface_format = supported_surface_formats[0];

		/* Define surface extent, this will be the same as the window size */
		const vk::Extent2D surface_extent{
			.width = window_width,
			.height = window_height,
		};

		vk::SurfaceCapabilitiesKHR surface_caps;
		std::tie(result, surface_caps) = ctx.physical_device.getSurfaceCapabilitiesKHR(ctx.surface);
		assert(result == vk::Result::eSuccess);

		/* Select the swapchain image count, default to 3 for now */
		constexpr u32 surface_count = 3;
		assert(surface_count >= surface_caps.minImageCount);
		assert(surface_count <= surface_caps.maxImageCount);

		/* Select pre-transform, hopefully identity is available in most drivers */
		constexpr vk::SurfaceTransformFlagBitsKHR pre_transform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
		assert(!!(pre_transform & surface_caps.supportedTransforms));

		/* Select pre-transform, hopefully opaque is available in most drivers */
		constexpr vk::CompositeAlphaFlagBitsKHR composite_alpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
		assert(!!(composite_alpha & surface_caps.supportedCompositeAlpha));

		/* Select present mode, fifo is supported by all drivers */
		constexpr vk::PresentModeKHR present_mode = vk::PresentModeKHR::eFifo;

		const vk::SwapchainCreateInfoKHR swapchain_info{
			.surface = ctx.surface,
			.minImageCount = surface_count,
			.imageFormat = surface_format.format,
			.imageColorSpace = surface_format.colorSpace,
			.imageExtent = surface_extent,
			.imageArrayLayers = 1,
			.imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
			.imageSharingMode = vk::SharingMode::eExclusive,
			.preTransform = pre_transform,
			.compositeAlpha = composite_alpha,
			.presentMode = present_mode,
			.clipped = true,
			.oldSwapchain = VK_NULL_HANDLE,
		};
		std::tie(result, swapchain) = ctx.device.createSwapchainKHR(swapchain_info);
		assert(result == vk::Result::eSuccess);

		std::vector<vk::Image> swapchain_images;
		std::tie(result, swapchain_images) = ctx.device.getSwapchainImagesKHR(swapchain);
		assert(result == vk::Result::eSuccess);

		per_frame_res.resize(surface_count);

		/* Transfer swapchain images */
		for (u32 i = 0; i < surface_count; i++)
		{
			per_frame_res[i].image = std::move(swapchain_images[i]);
		}

		/* Initialize per frame resources */
		for (PerFrameResources& res : per_frame_res)
		{
			/* Create command pool */
			const vk::CommandPoolCreateInfo pool_info{
				.queueFamilyIndex = ctx.queue_family_index,
			};
			std::tie(result, res.cmd_pool) = ctx.device.createCommandPool(pool_info);
			assert(result == vk::Result::eSuccess);

			/* Create command buffer */
			const vk::CommandBufferAllocateInfo allocate_info{
				.commandPool = res.cmd_pool,
				.level = vk::CommandBufferLevel::ePrimary,
				.commandBufferCount = 1,
			};
			std::vector<vk::CommandBuffer> cmd_buffers;
			std::tie(result, cmd_buffers) = ctx.device.allocateCommandBuffers(allocate_info);
			assert(result == vk::Result::eSuccess);
			res.cmd_buf = std::move(cmd_buffers[0]);

			/* Create fence */
			const vk::FenceCreateInfo fence_info{};
			std::tie(result, res.frame_complete_fence) = ctx.device.createFence(fence_info);
			assert(result == vk::Result::eSuccess);

			/* Create semaphores */
			const vk::SemaphoreCreateInfo sem_info{};
			std::tie(result, res.image_acquire_sem) = ctx.device.createSemaphore(sem_info);
			assert(result == vk::Result::eSuccess);
			std::tie(result, res.image_release_sem) = ctx.device.createSemaphore(sem_info);
			assert(result == vk::Result::eSuccess);

			/* Create image views */
			const vk::ImageViewCreateInfo image_view_info{
				.image = res.image,
				.viewType = vk::ImageViewType::e2D,
				.format = surface_format.format,
				.subresourceRange = {
					.aspectMask = vk::ImageAspectFlagBits::eColor,
					.baseMipLevel = 0,
					.levelCount = 1,
					.baseArrayLayer = 0,
					.layerCount = 1,
				},
			};
			std::tie(result, res.image_view) = ctx.device.createImageView(image_view_info);
			assert(result == vk::Result::eSuccess);
		}
	}

	void FrameManager::StartFrame(GraphicsContext& ctx)
	{
		vk::Result result = vk::Result::eSuccess;

		/* Acquire the new image and get the image index */
		u32 new_image_index = 0;
		result = ctx.device.acquireNextImageKHR(swapchain, UINT64_MAX, per_frame_res[current_frame_index].image_acquire_sem, VK_NULL_HANDLE, &new_image_index);
		assert(result == vk::Result::eSuccess);

		/* Wait for the last frame using this image index to finish (Most likely finished already) */
		PerFrameResources& new_frame_res = per_frame_res[new_image_index];
		if (new_frame_res.has_fence_signal)
		{
			result = ctx.device.waitForFences(new_frame_res.frame_complete_fence, true, UINT64_MAX);
			assert(result == vk::Result::eSuccess);

			result = ctx.device.resetFences(new_frame_res.frame_complete_fence);
			assert(result == vk::Result::eSuccess);
		}

		/* Swap over the acquire semaphore */
		std::swap(per_frame_res[current_frame_index].image_acquire_sem, new_frame_res.image_acquire_sem);

		/* Reset command pool */
		result = ctx.device.resetCommandPool(new_frame_res.cmd_pool);
		assert(result == vk::Result::eSuccess);

		/* Update frame index */
		current_frame_index = new_image_index;
	}

	void FrameManager::EndFrame(GraphicsContext& ctx)
	{
		vk::Result result = vk::Result::eSuccess;

		PerFrameResources& frame_res = per_frame_res[current_frame_index];

		/* Submit command buffer */
		const vk::SubmitInfo submit_info{
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &frame_res.image_acquire_sem,
			.commandBufferCount = 1,
			.pCommandBuffers = &frame_res.cmd_buf,
			.signalSemaphoreCount = 1,
			.pSignalSemaphores = &frame_res.image_release_sem,
		};
		result = ctx.queue.submit(submit_info, frame_res.frame_complete_fence);
		assert(result == vk::Result::eSuccess);
		frame_res.has_fence_signal = true;

		/* Present the image */
		vk::PresentInfoKHR present_info{
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &frame_res.image_release_sem,
			.swapchainCount = 1,
			.pSwapchains = &swapchain,
			.pImageIndices = &current_frame_index,
		};
		result = ctx.queue.presentKHR(present_info);
		assert(result == vk::Result::eSuccess);
	}
}