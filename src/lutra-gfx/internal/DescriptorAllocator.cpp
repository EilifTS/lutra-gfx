#include "DescriptorAllocator.h"

#include <cstdlib>
#include <iostream>

namespace lgx
{
	static constexpr u32 max_sets = 100;
	static constexpr u32 max_descriptors = 1024;

	DescriptorAllocator::DescriptorAllocator(GraphicsContextInternal& ctx)
		: dev(*ctx.device)
	{
		pools.push_back(createPool());
	}

	vk::UniqueDescriptorPool DescriptorAllocator::createPool() const
	{
		/* For now, just use some large enough values and avoid any complexity */
		const vk::DescriptorPoolSize sizes[] = {
			{ .type = vk::DescriptorType::eUniformBuffer, .descriptorCount = max_descriptors },
			{ .type = vk::DescriptorType::eStorageBuffer, .descriptorCount = max_descriptors },
			{ .type = vk::DescriptorType::eSampledImage, .descriptorCount = max_descriptors },
			{ .type = vk::DescriptorType::eStorageImage, .descriptorCount = max_descriptors },
			{ .type = vk::DescriptorType::eSampler, .descriptorCount = max_descriptors },
		};

		const vk::DescriptorPoolCreateInfo pool_info{
			.maxSets = max_sets,
			.poolSizeCount = sizeof(sizes) / sizeof(vk::DescriptorPoolSize),
			.pPoolSizes = sizes,
		};
		return VkCheck(dev.createDescriptorPoolUnique(pool_info));
	}

	vk::DescriptorSet DescriptorAllocator::Alloc(vk::DescriptorSetLayout layout)
	{
		bool is_empty_pool = false;
		while (true)
		{
			const vk::DescriptorSetAllocateInfo alloc_info{
				.descriptorPool = *pools[current_pool],
				.descriptorSetCount = 1,
				.pSetLayouts = &layout,
			};
			/* Not the std::vector overload, since that one asserts on a full pool before it can be handled here */
			vk::DescriptorSet set{};
			const vk::Result result = dev.allocateDescriptorSets(&alloc_info, &set);
			if (result == vk::Result::eSuccess)
			{
				return set;
			}

			/* Anything but a full pool is a real error, and a layout that does not fit in an empty
			   pool would otherwise keep adding pools forever */
			if ((result != vk::Result::eErrorOutOfPoolMemory && result != vk::Result::eErrorFragmentedPool) || is_empty_pool)
			{
				std::cerr << "Failed to allocate a descriptor set (" << vk::to_string(result) << ")" << std::endl;
				std::abort();
			}

			/* This pool is full, continue in the next one and create it if there is none */
			current_pool++;
			if (current_pool == pools.size())
			{
				pools.push_back(createPool());
				is_empty_pool = true;
			}
		}
	}

	void DescriptorAllocator::Reset()
	{
		for (const vk::UniqueDescriptorPool& pool : pools)
		{
			VkCheck(dev.resetDescriptorPool(*pool));
		}
		current_pool = 0;
	}
}