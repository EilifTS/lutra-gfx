#pragma once
#include <memory>
#include "GraphicsContext.h"
#include "CommonStructs.h"

namespace lgx
{
	class RenderTargetInternal;

	class RenderTarget
	{
	public:
		RenderTarget();
		RenderTarget(GraphicsContext& ctx, u32 width, u32 height);
		~RenderTarget();

		RenderTarget(const RenderTarget&) = delete;
		RenderTarget& operator=(const RenderTarget&) = delete;

		RenderTarget(RenderTarget&&);
		RenderTarget& operator=(RenderTarget&&);

		u32 Width() const;
		u32 Height() const;

		TextureView DefaultView() const;

#ifdef USE_IMGUI
		void* GetImGuiID() const;
#endif

		std::unique_ptr<RenderTargetInternal> internal{};
	};
}
