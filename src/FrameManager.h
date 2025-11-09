#pragma once
#include "GraphicsContext.h"

namespace efvk
{
	struct PerFrameResources
	{
		vk::CommandPool cmd_pool{};
		vk::CommandBuffer cmd_buf{};
		bool has_fence_signal{ false };
		vk::Fence frame_complete_fence{};
		vk::Semaphore image_acquire_sem{};
		vk::Semaphore image_release_sem{};
		vk::Image image{};
		vk::ImageView image_view{};
	};

	class FrameManager
	{
	public:
		FrameManager(GraphicsContext& ctx, u32 window_width, u32 window_height);

		void StartFrame(GraphicsContext& ctx);
		void EndFrame(GraphicsContext& ctx);

	private:
		vk::SwapchainKHR swapchain{};
		std::vector<PerFrameResources> per_frame_res{};

		u32 current_frame_index = 0;

	};
}