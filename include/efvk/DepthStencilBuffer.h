#pragma once
#include <memory>
#include "GraphicsContext.h"
#include <efvk/math/IntDefs.h>

namespace efvk
{
	class DepthStencilBufferInternal;

	enum class DepthStencilFormat
	{
		D32,
	};

	class DepthStencilBuffer
	{
	public:
		DepthStencilBuffer();
		DepthStencilBuffer(GraphicsContext& ctx, u32 width, u32 height, DepthStencilFormat format);
		~DepthStencilBuffer();

		DepthStencilBuffer(const DepthStencilBuffer&) = delete;
		DepthStencilBuffer(DepthStencilBuffer&&);
		DepthStencilBuffer& operator=(const DepthStencilBuffer&) = delete;
		DepthStencilBuffer& operator=(DepthStencilBuffer&&);

		std::unique_ptr<DepthStencilBufferInternal> internal{};

		u32 Width() const;
		u32 Height() const;
	};
}