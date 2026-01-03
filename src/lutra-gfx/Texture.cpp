#include <lutra-gfx/Texture.h>

#include "internal/TextureInternal.h"

namespace lgx
{
	Texture::Texture(){}

	Texture::Texture(GraphicsContext& ctx, const Image& image)
	{
		internal = std::make_unique<TextureInternal>(*ctx.internal, image);
	}

	Texture::~Texture() {}
	Texture::Texture(Texture&&) = default;
	Texture& Texture::operator=(Texture&&) = default;

	u32 Texture::Width() const {
		return internal->width;
	}

	u32 Texture::Height() const {
		return internal->height;
	}

	TextureView Texture::DefaultView() const {
		return *internal->view;
	}

#ifdef USE_IMGUI
	void* Texture::GetImGuiID() const {
		return static_cast<void*>(internal->imgui_set);
	}
#endif
}