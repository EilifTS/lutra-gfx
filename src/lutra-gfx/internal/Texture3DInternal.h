#pragma once
#include <vector>

#include <lutra-gfx/Texture3D.h>
#include "VulkanHPP.h"
#include "GraphicsContextInternal.h"

namespace lgx
{
	class Texture3DInternal
	{
	public:
		Texture3DInternal() {};
		Texture3DInternal(GraphicsContextInternal& ctx, const Texture3DInfo& info);

		VMAImage vma_image{};
		TextureFormat texture_format{};
		u32 width{};
		u32 height{};
		u32 depth{};
		u32 mip_count{};

		/* Every mip, for sampling */
		vk::UniqueImageView view{};

		/* One view per mip, a storage image can only be one mip */
		std::vector<vk::UniqueImageView> mip_views{};
	};
}
