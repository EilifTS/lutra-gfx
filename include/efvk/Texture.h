#pragma once
#include <memory>

#include "GraphicsContext.h"
#include "Image.h"

namespace efvk
{
	class TextureInternal;

	class Texture
	{
	public:
		Texture();
		Texture(GraphicsContext& ctx, const Image& image);
		~Texture();

		Texture(const Texture&) = delete;
		Texture(Texture&&);
		Texture& operator=(const Texture&) = delete;
		Texture& operator=(Texture&&);

		std::unique_ptr<TextureInternal> internal{};

		u32 Width() const;
		u32 Height() const;

#ifdef USE_IMGUI
		void* GetImGuiID() const;
#endif
	};
}