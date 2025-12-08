#pragma once
#include "VulkanHPP.h"
#include <efvk/math/IntDefs.h>

namespace efvk
{
	struct Texture::Impl
	{
		vk::Device dev{};

		VMAImage image{};
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