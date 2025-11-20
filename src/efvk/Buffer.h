#pragma once
#include <efvk/GraphicsContext.h>
#include <efvk/math/IntDefs.h>
#include "GraphicsContextImpl.h"
#include "VulkanHPP.h"

namespace efvk
{
	class Buffer
	{
	public:
		Buffer() {};
		Buffer(GraphicsContext::Impl& ctx, u32 size, vk::BufferUsageFlags usage, VmaAllocationCreateFlags vma_flags);
		~Buffer();

		Buffer(const Buffer&) = delete;
		Buffer(Buffer&& rhs) noexcept
		{
			allocator = rhs.allocator;
			allocation = rhs.allocation;
			buffer = rhs.buffer;
			rhs.allocator = VK_NULL_HANDLE;
			rhs.allocation = VK_NULL_HANDLE;
			rhs.buffer = VK_NULL_HANDLE;
		}
		Buffer& operator=(const Buffer&) = delete;
		Buffer& operator=(Buffer&& rhs) noexcept
		{
			allocator = rhs.allocator;
			allocation = rhs.allocation;
			buffer = rhs.buffer;
			rhs.allocator = VK_NULL_HANDLE;
			rhs.allocation = VK_NULL_HANDLE;
			rhs.buffer = VK_NULL_HANDLE;
			return *this;
		}

		u64 Size() const { return size; }
		void* Map(vk::Device device);
		void Unmap(vk::Device device);

		VmaAllocator allocator{};
		VmaAllocation allocation{};
		vk::Buffer buffer{};

	private:
		u64 size{};
	};
}