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
	};
}