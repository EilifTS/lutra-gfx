#pragma once
#include <algorithm>
#include <memory>

#include "GraphicsContext.h"
#include "CommonStructs.h"

namespace lgx
{
	class Texture3DInternal;

	/* The size of a mip level, which is half of the one before it down to one texel */
	inline u32 CalculateMipSize(u32 size, u32 mip)
	{
		const u32 mip_size = size >> mip;
		return mip_size > 0 ? mip_size : 1;
	}

	/* The number of mips it takes to get down to a single texel, for Texture3DInfo::mip_count */
	inline u32 CalculateMipCount(u32 width, u32 height, u32 depth)
	{
		u32 mip_count = 1;
		for (u32 largest_side = std::max(width, std::max(height, depth)); largest_side > 1; largest_side >>= 1)
		{
			mip_count++;
		}
		return mip_count;
	}

	struct Texture3DInfo
	{
		u32 width{};
		u32 height{};
		u32 depth{};
		TextureFormat format{ TextureFormat::RGBA8 };

		/* How many mips the texture has, at least one. CalculateMipCount gives the whole chain. */
		u32 mip_count{ 1 };

		/* Lets compute shaders write to the texture, by binding a mip with BindStorageImage.
		   The format has to support it on the GPU, which R8Snorm and R8Unorm only do on GPUs with
		   extended storage image formats, nearly all of the desktop ones. */
		bool storage{ false };
	};

	/* A 3D image that can be sampled in shaders, with mips, and written by compute shaders if it was created with storage.
	   Like every image in this library it stays in VK_IMAGE_LAYOUT_GENERAL, so the only synchronization needed is
	   CommandBuffer::Barrier. Its content is undefined until it has been written. */
	class Texture3D
	{
	public:
		Texture3D();
		Texture3D(GraphicsContext& ctx, const Texture3DInfo& info);
		~Texture3D();

		Texture3D(const Texture3D&) = delete;
		Texture3D(Texture3D&&);
		Texture3D& operator=(const Texture3D&) = delete;
		Texture3D& operator=(Texture3D&&);

		std::unique_ptr<Texture3DInternal> internal{};

		u32 Width() const;
		u32 Height() const;
		u32 Depth() const;
		u32 MipCount() const;
		TextureFormat Format() const;

		/* Every mip, to sample with CommandBuffer::BindTexture. The mip is picked in the shader with SampleLevel. */
		TextureView DefaultView() const;

		/* A single mip, for CommandBuffer::BindStorageImage, which can not take all of them. It can also be sampled. */
		TextureView MipView(u32 mip) const;
	};
}
