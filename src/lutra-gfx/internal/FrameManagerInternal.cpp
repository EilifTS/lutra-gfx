#include "FrameManagerInternal.h"
#include "GraphicsContextInternal.h"
#include "CommonHelpers.h"

#include <algorithm>
#include <iostream>

namespace lgx
{
	static void image_barrier(vk::CommandBuffer cmd_buf, vk::Image image, vk::PipelineStageFlags src_stage, vk::PipelineStageFlags dst_stage, vk::AccessFlags src_access, vk::AccessFlagBits dst_access)
	{
		vk::ImageSubresourceRange range{
			.aspectMask = vk::ImageAspectFlagBits::eColor,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1,
		};

		const vk::ImageMemoryBarrier image_barrier{
			.srcAccessMask = src_access,
			.dstAccessMask = dst_access,
			.oldLayout = vk::ImageLayout::eGeneral,
			.newLayout = vk::ImageLayout::eGeneral,
			.srcQueueFamilyIndex = vk::QueueFamilyIgnored,
			.dstQueueFamilyIndex = vk::QueueFamilyIgnored,
			.image = image,
			.subresourceRange = range,
		};

		cmd_buf.pipelineBarrier(
			src_stage,
			dst_stage,
			vk::DependencyFlagBits::eByRegion,
			{},
			{},
			image_barrier
		);
	}

	FrameManagerInternal::FrameManagerInternal(GraphicsContextInternal& ctx, u32 window_width, u32 window_height)
	{
		dev = *ctx.device;
		CreateSwapchain(ctx, window_width, window_height);
	}

	void FrameManagerInternal::CreateSwapchain(GraphicsContextInternal& ctx, u32 width, u32 height)
	{
		/* The surface format was selected when the context was created, since pipelines depend on it. */
		const vk::SurfaceFormatKHR surface_format{ convert_color_format(ctx.swapchain_color_format), vk::ColorSpaceKHR::eSrgbNonlinear };

		auto surface_caps = VkCheck(ctx.physical_device.getSurfaceCapabilitiesKHR(*ctx.surface));

		/* If the surface dictates its extent, use it. It is in pixels, whereas the requested size
		   is in window (screen) coordinates, which differ on high-DPI displays such as macOS Retina.
		   Otherwise (extent is the special value 0xFFFFFFFF) clamp the requested extent to what the
		   surface supports (relevant after a resize). */
		const vk::Extent2D surface_extent = surface_caps.currentExtent.width != 0xFFFFFFFFu
			? surface_caps.currentExtent
			: vk::Extent2D{
				.width = std::clamp(width, surface_caps.minImageExtent.width, surface_caps.maxImageExtent.width),
				.height = std::clamp(height, surface_caps.minImageExtent.height, surface_caps.maxImageExtent.height),
			};

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
			.imageUsage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst,
			.imageSharingMode = vk::SharingMode::eExclusive,
			.preTransform = pre_transform,
			.compositeAlpha = composite_alpha,
			.presentMode = present_mode,
			.clipped = true,
			/* Null on the very first call (default-constructed UniqueHandle), the retiring
			   swapchain on any later recreation. */
			.oldSwapchain = *swapchain,
		};
		swapchain = VkCheck(ctx.device->createSwapchainKHRUnique(swapchain_info));

		window_width = surface_extent.width;
		window_height = surface_extent.height;

		std::vector<vk::Image> swapchain_images = VkCheck(ctx.device->getSwapchainImagesKHR(*swapchain));
		assert(swapchain_images.size() >= surface_count);

		per_frame_res.resize(surface_count);

		/* Transfer swapchain images */
		{
			CommandBufferInternal cmd_buf(ctx);

			for (u32 i = 0; i < surface_count; i++)
			{
				per_frame_res[i].image = swapchain_images[i];
				change_layout(*cmd_buf.cmd_buf, swapchain_images[i], vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR, vk::ImageAspectFlagBits::eColor);
			}

			SubmitAndWaitInternal(ctx, cmd_buf);
		}

		/* Initialize per frame resources */
		for (PerFrameResources& res : per_frame_res)
		{
			/* Create command buffer, fence and semaphore once; these aren't tied to the
			   swapchain's resolution so they're left alone on recreation. */
			if (res.cmd_buf.internal == nullptr)
			{
				res.cmd_buf.internal = std::make_unique<CommandBufferInternal>(ctx);
				res.frame_complete_fence = VkCheck(ctx.device->createFenceUnique({}));
				res.image_release_sem = VkCheck(ctx.device->createSemaphoreUnique({}));
			}

			/* (Re)create the image view, since it's tied to a specific swapchain image */
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
			res.image_view = VkCheck(ctx.device->createImageViewUnique(image_view_info));
		}
	}

	bool FrameManagerInternal::RecreateSwapchain(GraphicsContextInternal& ctx)
	{
		const auto surface_caps = VkCheck(ctx.physical_device.getSurfaceCapabilitiesKHR(*ctx.surface));

		/* Window is minimized (zero-sized framebuffer): nothing sensible to render yet. */
		if (surface_caps.currentExtent.width == 0 || surface_caps.currentExtent.height == 0)
		{
			return false;
		}

		/* Make sure no in-flight work still references the old swapchain's images/views
		   before we replace them. */
		VkCheck(ctx.device->waitIdle());

		CreateSwapchain(ctx, surface_caps.currentExtent.width, surface_caps.currentExtent.height);
		return true;
	}

	FrameManagerInternal::~FrameManagerInternal()
	{
		VkCheck(dev.waitIdle());
	}

	bool FrameManagerInternal::StartFrame(GraphicsContextInternal& ctx)
	{
		if (needs_recreate)
		{
			if (!RecreateSwapchain(ctx))
			{
				/* Still nothing sensible to render into (e.g. window still minimized) */
				return false;
			}
			needs_recreate = false;
		}

		/* Find an acquire semaphore */
		vk::UniqueSemaphore acquire_semahore{};
		if (free_semaphore_queue.size() == 0)
		{
			acquire_semahore = VkCheck(ctx.device->createSemaphoreUnique({}));
		}
		else
		{
			acquire_semahore = std::move(free_semaphore_queue.back());
			free_semaphore_queue.pop_back();
		}

		/* Acquire the new image and get the image index */
		u32 new_image_index = 0;
		const vk::Result acquire_result = ctx.device->acquireNextImageKHR(*swapchain, UINT64_MAX, *acquire_semahore, VK_NULL_HANDLE, &new_image_index);

		if (acquire_result == vk::Result::eErrorOutOfDateKHR)
		{
			/* No image was actually acquired, so the semaphore was never signalled - safe to reuse. */
			free_semaphore_queue.push_back(std::move(acquire_semahore));
			needs_recreate = true;
			return false;
		}
		assert(acquire_result == vk::Result::eSuccess || acquire_result == vk::Result::eSuboptimalKHR);

		/* Wait for the last frame using this image index to finish (Most likely finished already) */
		PerFrameResources& new_frame_res = per_frame_res[new_image_index];
		if (new_frame_res.has_fence_signal)
		{
			const vk::Result wait_result = ctx.device->waitForFences(*new_frame_res.frame_complete_fence, true, UINT64_MAX);
			assert(wait_result == vk::Result::eSuccess);
			new_frame_res.has_fence_signal = false;

			VkCheck(ctx.device->resetFences(*new_frame_res.frame_complete_fence));

			/* This semaphore must have been signalled so we can free it. */
			free_semaphore_queue.push_back(std::move(new_frame_res.image_acquire_sem));
		}

		/* Store away the acquire semaphore for later */
		assert(new_frame_res.image_acquire_sem.get() == nullptr);
		new_frame_res.image_acquire_sem = std::move(acquire_semahore);

		/* Update frame index */
		current_frame_index = new_image_index;

		/* Reset command buffer */
		new_frame_res.cmd_buf.Reset();

		/* Transition the image layout to allow rendering */
		change_layout(*new_frame_res.cmd_buf.internal->cmd_buf, new_frame_res.image, vk::ImageLayout::ePresentSrcKHR, vk::ImageLayout::eGeneral, vk::ImageAspectFlagBits::eColor);

		/* Clear image */
		vk::ClearColorValue clear_color_value{};
		clear_color_value.float32[0] = 0.0f;
		clear_color_value.float32[1] = 0.0f;
		clear_color_value.float32[2] = 0.0f;
		clear_color_value.float32[3] = 1.0f;

		const vk::ImageSubresourceRange range{
			.aspectMask = vk::ImageAspectFlagBits::eColor,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1,
		};

		new_frame_res.cmd_buf.internal->cmd_buf->clearColorImage(new_frame_res.image, vk::ImageLayout::eGeneral, clear_color_value, range);
		image_barrier(*new_frame_res.cmd_buf.internal->cmd_buf, new_frame_res.image, vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eColorAttachmentWrite);

		return true;
	}

	void FrameManagerInternal::EndFrame(GraphicsContextInternal& ctx)
	{
		PerFrameResources& frame_res = per_frame_res[current_frame_index];

		/* Transition the image layout to prepare for present */
		change_layout(*frame_res.cmd_buf.internal->cmd_buf, frame_res.image, vk::ImageLayout::eGeneral, vk::ImageLayout::ePresentSrcKHR, vk::ImageAspectFlagBits::eColor);

		/* Submit command buffer */
		VkCheck(frame_res.cmd_buf.internal->cmd_buf->end());

		const vk::PipelineStageFlags wait_stage = vk::PipelineStageFlagBits::eColorAttachmentOutput;

		/* Using address of .get() is fine here, since it returns a reference*/
		const vk::SubmitInfo submit_info{
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &(frame_res.image_acquire_sem.get()),
			.pWaitDstStageMask = &wait_stage,
			.commandBufferCount = 1,
			.pCommandBuffers = &(frame_res.cmd_buf.internal->cmd_buf.get()),
			.signalSemaphoreCount = 1,
			.pSignalSemaphores = &(frame_res.image_release_sem.get()),
		};
		VkCheck(ctx.queue.submit(submit_info, *frame_res.frame_complete_fence));
		frame_res.has_fence_signal = true;

		/* Present the image */
		vk::PresentInfoKHR present_info{
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &(frame_res.image_release_sem.get()),
			.swapchainCount = 1,
			.pSwapchains = &(swapchain.get()),
			.pImageIndices = &current_frame_index,
		};

		/* Deliberately calling the pointer-taking overload here: the reference-taking one
		   throws on eErrorOutOfDateKHR (not in its accepted-result list), but that's an
		   expected, recoverable result after a resize - not an error we want to throw on. */
		const vk::Result present_result = ctx.queue.presentKHR(&present_info);
		if (present_result == vk::Result::eErrorOutOfDateKHR || present_result == vk::Result::eSuboptimalKHR)
		{
			/* Recreate before the next StartFrame instead of right now - the presented
			   image still needs to stay alive for the presentation engine to consume. */
			needs_recreate = true;
		}
		else
		{
			assert(present_result == vk::Result::eSuccess);
		}
	}
}