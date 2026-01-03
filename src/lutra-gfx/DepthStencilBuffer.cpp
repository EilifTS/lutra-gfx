#include <lutra-gfx/DepthStencilBuffer.h>

#include "internal/DepthStencilBufferInternal.h"
#include "internal/CommonHelpers.h"

namespace lgx
{
	DepthStencilBuffer::DepthStencilBuffer(){}

	DepthStencilBuffer::DepthStencilBuffer(GraphicsContext& ctx, u32 width, u32 height, DepthStencilFormat format)
	{
		internal = std::make_unique<DepthStencilBufferInternal>(*ctx.internal, width, height, convert_ds_format(format));
	}

	DepthStencilBuffer::~DepthStencilBuffer() {}
	DepthStencilBuffer::DepthStencilBuffer(DepthStencilBuffer&&) = default;
	DepthStencilBuffer& DepthStencilBuffer::operator=(DepthStencilBuffer&&) = default;

	u32 DepthStencilBuffer::Width() const {
		return internal->width;
	}

	u32 DepthStencilBuffer::Height() const {
		return internal->height;
	}

	TextureView DepthStencilBuffer::DefaultView() const {
		return *internal->view;
	}
}