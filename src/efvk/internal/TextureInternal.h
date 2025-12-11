#pragma once
#include "../VulkanHPP.h"
#include <efvk/GraphicsContext.h>
#include <efvk/Image.h>

namespace efvk
{
	class TextureInternal
	{
	public:
		TextureInternal() {};
		TextureInternal(GraphicsContext::Impl& ctx, const Image& image);
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