#include "Buffer.h"

namespace efvk
{
	Buffer::Buffer(GraphicsContext::Impl& ctx, u32 size, vk::BufferUsageFlags usage, VmaAllocationCreateFlags vma_flags)
		: size(size)
	{
		allocator = ctx.vma_allocator;

		const vk::BufferCreateInfo buffer_info{
			.size = size,
			.usage = usage,
		};

		VmaAllocationCreateInfo vma_info{};
		vma_info.usage = VMA_MEMORY_USAGE_AUTO;
		vma_info.flags = vma_flags;

		VkBuffer temp_buffer;
		const VkBufferCreateInfo vk_buf = buffer_info;
		VkResult result = vmaCreateBuffer(ctx.vma_allocator, &vk_buf, &vma_info, &temp_buffer, &allocation, nullptr);
		assert(result == VK_SUCCESS);
		buffer = temp_buffer;
	}

	Buffer::~Buffer()
	{
		if (buffer != VK_NULL_HANDLE)
		{
			vmaDestroyBuffer(allocator, buffer, allocation);
		}
	}

	void* Buffer::Map(vk::Device device)
	{
		void* mapped_ptr = nullptr;
		VkResult result = vmaMapMemory(allocator, allocation, &mapped_ptr);
		assert(result == VK_SUCCESS);
		return mapped_ptr;
	}

	void Buffer::Unmap(vk::Device device)
	{
		vmaUnmapMemory(allocator, allocation);
	}
}