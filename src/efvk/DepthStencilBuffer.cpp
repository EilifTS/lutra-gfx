#include <efvk/DepthStencilBuffer.h>

#include "internal/DepthStencilBufferInternal.h"

namespace efvk
{
	static vk::Format convert_ds_format(DepthStencilFormat format)
	{
		switch (format)
		{
		case DepthStencilFormat::D32: return vk::Format::eD32Sfloat;
		default: assert(false);
		}
		assert(false);
		return vk::Format::eUndefined;
	}

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
}