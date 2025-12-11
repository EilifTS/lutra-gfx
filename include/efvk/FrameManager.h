#pragma once
#include <memory>
#include <efvk/math/IntDefs.h>

#include "GraphicsContext.h"

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

		std::unique_ptr<FrameManagerInternal> internal{};
	};
}