#pragma once
#include <lutra-gfx/CommonStructs.h>
#include <lutra-gfx/CommandBuffer.h>

#include "GraphicsContextInternal.h"
#include "CommandBufferInternal.h"

namespace lgx
{
	struct PerFrameResources
	{
		CommandBuffer cmd_buf{};
		bool has_fence_signal{ false };
		vk::UniqueFence frame_complete_fence{};
		vk::UniqueSemaphore image_acquire_sem{};
		vk::UniqueSemaphore image_release_sem{};
		vk::Image image{};
		vk::UniqueImageView image_view{};
	};

	class FrameManagerInternal
	{
	public:
		FrameManagerInternal(GraphicsContextInternal& ctx, u32 window_width, u32 window_height);
		~FrameManagerInternal();

		/* Returns false if no frame was started (e.g. the swapchain needed to be recreated
		   because of a resize, or the window is currently minimized) - the caller should skip
		   rendering and EndFrame for this iteration and just try again next frame. */
		bool StartFrame(GraphicsContextInternal& ctx);
		void EndFrame(GraphicsContextInternal& ctx);

		CommandBuffer& GetCurrentCommandBuffer()
		{
			return per_frame_res[current_frame_index].cmd_buf;
		}

		vk::Image GetCurrentImage()
		{
			return per_frame_res[current_frame_index].image;
		}

		vk::ImageView GetCurrentImageView()
		{
			return *per_frame_res[current_frame_index].image_view;
		}

		vk::Device dev{};
		vk::UniqueSwapchainKHR swapchain{};
		std::vector<vk::UniqueSemaphore> free_semaphore_queue{};
		std::vector<PerFrameResources> per_frame_res{};

		u32 window_width{};
		u32 window_height{};
		u32 current_frame_index = 0;

	private:
		void CreateSwapchain(GraphicsContextInternal& ctx, u32 width, u32 height);

		/* Returns false if recreation couldn't happen yet (e.g. the window is minimized,
		   giving a zero-sized surface) - the caller should try again next frame. */
		bool RecreateSwapchain(GraphicsContextInternal& ctx);

		bool needs_recreate{ false };
	};
}