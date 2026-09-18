#pragma once
#include "VulkanHPP.h"
#include "GraphicsContextInternal.h"

namespace lgx
{
	class RenderTargetInternal
	{
	public:
		RenderTargetInternal() {};
		RenderTargetInternal(GraphicsContextInternal& ctx, u32 width, u32 height);
		~RenderTargetInternal();

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
