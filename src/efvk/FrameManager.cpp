#include <efvk/FrameManager.h>

#include "internal/FrameManagerInternal.h"

namespace efvk
{
	FrameManager::FrameManager(GraphicsContext& ctx, u32 window_width, u32 window_height)
	{
		internal = std::make_unique<FrameManagerInternal>(*ctx.internal, window_width, window_height);
	}

	FrameManager::~FrameManager(){}

	FrameManager::FrameManager(FrameManager&&) = default;
	FrameManager& FrameManager::operator=(FrameManager&&) = default;

	void FrameManager::StartFrame(GraphicsContext& ctx)
	{
		internal->StartFrame(*ctx.internal);
	}

	void FrameManager::EndFrame(GraphicsContext& ctx)
	{
		internal->EndFrame(*ctx.internal);
	}
}