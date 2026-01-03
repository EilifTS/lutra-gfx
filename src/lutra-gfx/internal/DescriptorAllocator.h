#pragma once
#include <lutra-gfx/GraphicsContext.h>
#include "GraphicsContextInternal.h"

namespace lgx
{
	class DescriptorAllocator
	{
	public:
		DescriptorAllocator() {};
		DescriptorAllocator(GraphicsContextInternal& ctx);

		DescriptorAllocator(const DescriptorAllocator&) = delete;
		DescriptorAllocator(DescriptorAllocator&&) = default;
		DescriptorAllocator& operator=(const DescriptorAllocator&) = delete;
		DescriptorAllocator& operator=(DescriptorAllocator&&) = default;

		vk::DescriptorSet Alloc(vk::DescriptorSetLayout layout);
		void Reset();

	private:
		vk::Device dev{};
		vk::UniqueDescriptorPool pool{};
	};
}