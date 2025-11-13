#pragma once
#include "GraphicsContext.h"

namespace efvk
{
	struct PerFrameResources
	{
		vk::UniqueCommandPool cmd_pool{};
		vk::UniqueCommandBuffer cmd_buf{};
		bool has_fence_signal{ false };
		vk::UniqueFence frame_complete_fence{};
		vk::UniqueSemaphore image_acquire_sem{};
		vk::UniqueSemaphore image_release_sem{};
		vk::Image image{};
		vk::UniqueImageView image_view{};
	};

	class FrameManager
	{
	public:
		FrameManager(GraphicsContext& ctx, u32 window_width, u32 window_height);
		~FrameManager();

		void StartFrame(GraphicsContext& ctx);
		void EndFrame(GraphicsContext& ctx);

	private:
		vk::Device dev{};

		vk::UniqueSwapchainKHR swapchain{};

		std::vector<vk::UniqueSemaphore> free_semaphore_queue{};
		std::vector<PerFrameResources> per_frame_res{};

		u32 current_frame_index = 0;

	};
}