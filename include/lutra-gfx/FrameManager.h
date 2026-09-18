#pragma once
#include <memory>

#include "CommonStructs.h"
#include "GraphicsContext.h"
#include "CommandBuffer.h"
#include "Texture.h"

namespace lgx
{
	class FrameManagerInternal;

	class FrameManager
	{
	public:
		FrameManager(GraphicsContext& ctx, u32 window_width, u32 window_height);
		~FrameManager();

		FrameManager(const FrameManager&) = delete;
		FrameManager(FrameManager&&);
		FrameManager& operator=(const FrameManager&) = delete;
		FrameManager& operator=(FrameManager&&);

		/* Returns false if no frame was started (swapchain needed recreating, e.g. after a
		   resize, or the window is currently minimized) - skip rendering and EndFrame this
		   iteration and just call StartFrame again next frame. */
		bool StartFrame(GraphicsContext& ctx);
		void EndFrame(GraphicsContext& ctx);

		u32 FrameWidth() const;
		u32 FrameHeight() const;

		CommandBuffer& GetCurrentCommandBuffer();
		TextureView GetCurrentTetureView();

		std::unique_ptr<FrameManagerInternal> internal{};
	};
}