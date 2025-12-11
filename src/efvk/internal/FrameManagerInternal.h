#pragma once
#include <efvk/math/IntDefs.h>

#include "GraphicsContextInternal.h"
#include "../CommandBuffer.h"

namespace efvk
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

		void StartFrame(GraphicsContextInternal& ctx);
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
	};
}