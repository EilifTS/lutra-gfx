#include "DescriptorAllocator.h"

namespace lgx
{
	static constexpr u32 max_sets = 100;
	static constexpr u32 max_descriptors = 1024;

	DescriptorAllocator::DescriptorAllocator(GraphicsContextInternal& ctx)
		: dev(*ctx.device)
	{
		/* For now, just use some large enough values and avoid any complexity */
		const vk::DescriptorPoolSize sizes[] = {
			{ .type = vk::DescriptorType::eStorageBuffer, .descriptorCount = max_descriptors },
			{ .type = vk::DescriptorType::eSampledImage, .descriptorCount = max_descriptors },
			{ .type = vk::DescriptorType::eSampler, .descriptorCount = max_descriptors },
		};

		const vk::DescriptorPoolCreateInfo pool_info{
			.maxSets = max_sets,
			.poolSizeCount = sizeof(sizes) / sizeof(vk::DescriptorPoolSize),
			.pPoolSizes = sizes,
		};
		pool = ctx.device->createDescriptorPoolUnique(pool_info);
	}

	vk::DescriptorSet DescriptorAllocator::Alloc(vk::DescriptorSetLayout layout)
	{
		const vk::DescriptorSetAllocateInfo alloc_info{
			.descriptorPool = *pool,
			.descriptorSetCount = 1,
			.pSetLayouts = &layout,
		};
		std::vector<vk::DescriptorSet> sets = dev.allocateDescriptorSets(alloc_info);

		return sets[0];
	}

	void DescriptorAllocator::Reset()
	{
		dev.resetDescriptorPool(*pool);
	}
}