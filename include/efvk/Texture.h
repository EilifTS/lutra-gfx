#pragma once
#include <memory>

#include "GraphicsContext.h"
#include "Image.h"

namespace efvk
{
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

		struct Impl;
		std::unique_ptr<Impl> pimpl{};

		u32 Width() const;
		u32 Height() const;

	private:
	};
}