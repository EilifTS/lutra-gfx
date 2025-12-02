#pragma once
#include <efvk/GraphicsContext.h>
#include "GraphicsContextImpl.h"

namespace efvk
{
	class DescriptorAllocator
	{
	public:
		DescriptorAllocator() {};
		DescriptorAllocator(GraphicsContext::Impl& ctx);

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