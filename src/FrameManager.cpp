#include "FrameManager.h"

namespace efvk
{
	static vk::SurfaceFormatKHR select_surface_format(vk::PhysicalDevice phys_dev, vk::SurfaceKHR surface)
	{
		/* No special logic for now, just choose the first available */
		std::vector<vk::SurfaceFormatKHR> supported_surface_formats = phys_dev.getSurfaceFormatsKHR(surface);
		assert(supported_surface_formats.size() > 0);
		return supported_surface_formats[0];
	}

	static void change_layout(vk::CommandBuffer cmd_buf, vk::Image image, u32 queue_family_index, vk::ImageLayout old_layout, vk::ImageLayout new_layout)
	{
		vk::ImageSubresourceRange range{
			.aspectMask = vk::ImageAspectFlagBits::eColor,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1,
		};

		const vk::ImageMemoryBarrier image_barrier{
			.srcAccessMask = vk::AccessFlagBits::eNone,
			.dstAccessMask = vk::AccessFlagBits::eNone,
			.oldLayout = old_layout,
			.newLayout = new_layout,
			.srcQueueFamilyIndex = queue_family_index,
			.dstQueueFamilyIndex = queue_family_index,
			.image = image,
			.subresourceRange = range,
		};

		cmd_buf.pipelineBarrier(
			vk::PipelineStageFlagBits::eAllCommands,
			vk::PipelineStageFlagBits::eAllCommands,
			vk::DependencyFlagBits::eByRegion,
			{},
			{},
			image_barrier
		);
	}

	FrameManager::FrameManager(GraphicsContext& ctx, u32 window_width, u32 window_height)
	{
		vk::Result result = vk::Result::eSuccess;
		dev = *ctx.device;

		/* Select a surface format. */
		const vk::SurfaceFormatKHR surface_format = select_surface_format(ctx.physical_device, *ctx.surface);

		/* Define surface extent, this will be the same as the window size */
		const vk::Extent2D surface_extent{
			.width = window_width,
			.height = window_height,
		};

		auto surface_caps = ctx.physical_device.getSurfaceCapabilitiesKHR(*ctx.surface);

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
			.surface = *ctx.surface,
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
		swapchain = ctx.device->createSwapchainKHRUnique(swapchain_info);
		assert(result == vk::Result::eSuccess);

		std::vector<vk::Image> swapchain_images = ctx.device->getSwapchainImagesKHR(*swapchain);

		per_frame_res.resize(surface_count);

		/* Transfer swapchain images */
		for (u32 i = 0; i < surface_count; i++)
		{
			per_frame_res[i].image = swapchain_images[i];
		}

		/* Initialize per frame resources */
		for (PerFrameResources& res : per_frame_res)
		{
			/* Create command pool */
			const vk::CommandPoolCreateInfo pool_info{
				.queueFamilyIndex = ctx.queue_family_index,
			};
			res.cmd_pool = ctx.device->createCommandPoolUnique(pool_info);

			/* Create command buffer */
			const vk::CommandBufferAllocateInfo allocate_info{
				.commandPool = *res.cmd_pool,
				.level = vk::CommandBufferLevel::ePrimary,
				.commandBufferCount = 1,
			};
			std::vector<vk::CommandBuffer> cmd_buffers;
			res.cmd_buf = std::move(ctx.device->allocateCommandBuffersUnique(allocate_info)[0]);

			/* Create fence */
			res.frame_complete_fence = ctx.device->createFenceUnique({});

			/* Create semaphores */
			res.image_release_sem = ctx.device->createSemaphoreUnique({});

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
			res.image_view = ctx.device->createImageViewUnique(image_view_info);
		}
	}

	FrameManager::~FrameManager()
	{
		dev.waitIdle();
	}

	void FrameManager::StartFrame(GraphicsContext& ctx)
	{
		vk::Result result = vk::Result::eSuccess;

		/* Find an acquire semaphore */
		vk::UniqueSemaphore acquire_semahore{};
		if (free_semaphore_queue.size() == 0)
		{
			acquire_semahore = ctx.device->createSemaphoreUnique({});
		}
		else
		{
			acquire_semahore = std::move(free_semaphore_queue.back());
			free_semaphore_queue.pop_back();
		}

		/* Acquire the new image and get the image index */
		u32 new_image_index = 0;
		result = ctx.device->acquireNextImageKHR(*swapchain, UINT64_MAX, *acquire_semahore, VK_NULL_HANDLE, &new_image_index);
		assert(result == vk::Result::eSuccess);

		/* Wait for the last frame using this image index to finish (Most likely finished already) */
		PerFrameResources& new_frame_res = per_frame_res[new_image_index];
		if (new_frame_res.has_fence_signal)
		{
			result = ctx.device->waitForFences(*new_frame_res.frame_complete_fence, true, UINT64_MAX);
			assert(result == vk::Result::eSuccess);
			new_frame_res.has_fence_signal = false;

			ctx.device->resetFences(*new_frame_res.frame_complete_fence);

			/* This semaphore must have been signalled so we can free it. */
			free_semaphore_queue.push_back(std::move(new_frame_res.image_acquire_sem));
		}

		/* Store away the acquire semaphore for later */
		assert(new_frame_res.image_acquire_sem.get() == nullptr);
		new_frame_res.image_acquire_sem = std::move(acquire_semahore);

		/* Update frame index */
		current_frame_index = new_image_index;

		/* Reset command pool */
		ctx.device->resetCommandPool(*new_frame_res.cmd_pool);

		/* Begin command buffer */
		const vk::CommandBufferBeginInfo begin_info{
			.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
		};
		new_frame_res.cmd_buf->begin(begin_info);

		/* Transition the image layout to allow rendering */
		change_layout(*new_frame_res.cmd_buf, new_frame_res.image, ctx.queue_family_index, vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);
	}

	void FrameManager::EndFrame(GraphicsContext& ctx)
	{
		PerFrameResources& frame_res = per_frame_res[current_frame_index];

		/* Transition the image layout to prepare for present */
		change_layout(*frame_res.cmd_buf, frame_res.image, ctx.queue_family_index, vk::ImageLayout::eGeneral, vk::ImageLayout::ePresentSrcKHR);

		/* Submit command buffer */
		frame_res.cmd_buf->end();

		const vk::PipelineStageFlags wait_stage = vk::PipelineStageFlagBits::eColorAttachmentOutput;

		const vk::SubmitInfo submit_info{
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &(frame_res.image_acquire_sem.get()),
			.pWaitDstStageMask = &wait_stage,
			.commandBufferCount = 1,
			.pCommandBuffers = &(frame_res.cmd_buf.get()),
			.signalSemaphoreCount = 1,
			.pSignalSemaphores = &(frame_res.image_release_sem.get()),
		};
		ctx.queue.submit(submit_info, *frame_res.frame_complete_fence);
		frame_res.has_fence_signal = true;

		/* Present the image */
		vk::PresentInfoKHR present_info{
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &(frame_res.image_release_sem.get()),
			.swapchainCount = 1,
			.pSwapchains = &(swapchain.get()),
			.pImageIndices = &current_frame_index,
		};
		const vk::Result result = ctx.queue.presentKHR(present_info);
		assert(result == vk::Result::eSuccess);
	}
}