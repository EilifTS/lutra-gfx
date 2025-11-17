#pragma once
#include <memory>

#include "GraphicsContext.h"

namespace efvk
{
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

		struct Impl;
		std::unique_ptr<Impl> pimpl{};

	private:
		u32 current_frame_index = 0;
	};
}