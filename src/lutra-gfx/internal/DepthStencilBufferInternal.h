#pragma once
#include "VulkanHPP.h"
#include "GraphicsContextInternal.h"

namespace lgx
{
	class DepthStencilBufferInternal
	{
	public:
		DepthStencilBufferInternal() {};
		DepthStencilBufferInternal(GraphicsContextInternal& ctx, u32 width, u32 height, vk::Format format);

		vk::Device dev{};

		VMAImage vma_image{};
		vk::Format format{};
		u32 width{};
		u32 height{};
		vk::UniqueImageView view{};
	};
}