#include "BufferInternal.h"

namespace efvk
{
	BufferInternal::BufferInternal(GraphicsContextInternal& ctx, u64 size, vk::BufferUsageFlags usage, VmaAllocationCreateFlags vma_flags)
		: size(size)
	{
		const vk::BufferCreateInfo buffer_info{
			.size = size,
			.usage = usage,
		};

		VmaAllocationCreateInfo vma_info{};
		vma_info.usage = VMA_MEMORY_USAGE_AUTO;
		vma_info.flags = vma_flags;

		buffer = VMACreateBuffer(ctx.vma_allocator, &buffer_info, &vma_info, nullptr);
	}

	void* BufferInternal::Map(vk::Device device)
	{
		void* mapped_ptr = nullptr;
		VkResult result = vmaMapMemory(buffer.GetAllocator(), buffer.GetAllocation(), &mapped_ptr);
		assert(result == VK_SUCCESS);
		return mapped_ptr;
	}

	void BufferInternal::Unmap(vk::Device device)
	{
		vmaUnmapMemory(buffer.GetAllocator(), buffer.GetAllocation());
	}
}