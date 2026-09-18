#include <lutra-gfx/RenderTarget.h>

#include "internal/RenderTargetInternal.h"

namespace lgx
{
	RenderTarget::RenderTarget() {}

	RenderTarget::RenderTarget(GraphicsContext& ctx, u32 width, u32 height)
	{
		internal = std::make_unique<RenderTargetInternal>(*ctx.internal, width, height);
	}

	RenderTarget::~RenderTarget() {}
	RenderTarget::RenderTarget(RenderTarget&&) = default;
	RenderTarget& RenderTarget::operator=(RenderTarget&&) = default;

	u32 RenderTarget::Width() const {
		return internal->width;
	}

	u32 RenderTarget::Height() const {
		return internal->height;
	}

	TextureView RenderTarget::DefaultView() const {
		return *internal->view;
	}

#ifdef USE_IMGUI
	void* RenderTarget::GetImGuiID() const {
		return static_cast<void*>(internal->imgui_set);
	}
#endif
}
