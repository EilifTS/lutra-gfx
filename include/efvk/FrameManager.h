#pragma once
#include <memory>
#include <efvk/math/IntDefs.h>

#include "GraphicsContext.h"
#include "CommandBuffer.h"
#include "Texture.h"

namespace efvk
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

		void StartFrame(GraphicsContext& ctx);
		void EndFrame(GraphicsContext& ctx);

		u32 FrameWidth() const;
		u32 FrameHeight() const;

		CommandBuffer& GetCurrentCommandBuffer();
		TextureView GetCurrentTetureView();

		std::unique_ptr<FrameManagerInternal> internal{};
	};
}