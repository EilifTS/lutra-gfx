#pragma once
#include <lutra-gfx/Image.h>
#include "VulkanHPP.h"
#include "GraphicsContextInternal.h"

namespace lgx
{
	class TextureInternal
	{
	public:
		TextureInternal() {};
		TextureInternal(GraphicsContextInternal& ctx, const Image& image);
		~TextureInternal();

#ifdef USE_IMGUI
		void* GetImGuiID() const;
#endif

		vk::Device dev{};

		VMAImage vma_image{};
		vk::Format format{};
		u32 width{};
		u32 height{};
		vk::UniqueImageView view{};

#ifdef USE_IMGUI
		vk::UniqueSampler imgui_sampler{};
		vk::DescriptorSet imgui_set{};
#endif
	};
}