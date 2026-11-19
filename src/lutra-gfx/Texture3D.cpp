#include <lutra-gfx/Texture3D.h>

#include "internal/Texture3DInternal.h"

namespace lgx
{
	Texture3D::Texture3D() {}

	Texture3D::Texture3D(GraphicsContext& ctx, const Texture3DInfo& info)
	{
		internal = std::make_unique<Texture3DInternal>(*ctx.internal, info);
	}

	Texture3D::~Texture3D() {}
	Texture3D::Texture3D(Texture3D&&) = default;
	Texture3D& Texture3D::operator=(Texture3D&&) = default;

	u32 Texture3D::Width() const { return internal->width; }
	u32 Texture3D::Height() const { return internal->height; }
	u32 Texture3D::Depth() const { return internal->depth; }
	u32 Texture3D::MipCount() const { return internal->mip_count; }
	TextureFormat Texture3D::Format() const { return internal->texture_format; }

	TextureView Texture3D::DefaultView() const
	{
		return *internal->view;
	}

	TextureView Texture3D::MipView(u32 mip) const
	{
		assert(mip < internal->mip_count);
		return *internal->mip_views[mip];
	}
}
