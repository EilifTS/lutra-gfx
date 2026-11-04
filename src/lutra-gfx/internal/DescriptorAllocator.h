#pragma once
#include <lutra-gfx/GraphicsContext.h>
#include "GraphicsContextInternal.h"

#include <vector>

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

		/* Allocates a set from the current pool. If the pool is full a new one is added, so the
		   number of sets that can be allocated between two calls to Reset is not limited. */
		vk::DescriptorSet Alloc(vk::DescriptorSetLayout layout);
		void Reset();

	private:
		vk::UniqueDescriptorPool createPool() const;

		vk::Device dev{};
		std::vector<vk::UniqueDescriptorPool> pools{};
		size_t current_pool{ 0 };
	};
}