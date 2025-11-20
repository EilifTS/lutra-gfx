#pragma once
#include <efvk/math/IntDefs.h>

#include "Buffer.h"
#include "GraphicsContextImpl.h"

namespace efvk
{
	struct BufferMemoryAllocation
	{
		vk::Buffer buffer{};
		VmaAllocation vma_allocation{};
		char* ptr{};
		u64 offset{};
		u64 size{};
	};

	class BufferMemoryAllocator
	{
	public:
		BufferMemoryAllocator() {};
		BufferMemoryAllocator(GraphicsContext::Impl& ctx, u64 chunk_size);
		BufferMemoryAllocator(const BufferMemoryAllocator&) = delete;
		BufferMemoryAllocator(BufferMemoryAllocator&&) = default;
		BufferMemoryAllocator& operator=(const BufferMemoryAllocator&) = delete;
		BufferMemoryAllocator& operator=(BufferMemoryAllocator&&) = default;
		~BufferMemoryAllocator();

		BufferMemoryAllocation Alloc(GraphicsContext::Impl& ctx, u64 size, u64 alignment);
		void Reset(GraphicsContext::Impl& ctx);

	private:
		GraphicsContext::Impl* ctx{};

		void* first_chunk_ptr = nullptr;
		u64 chunk_size{};
		std::vector<Buffer> chunks{};
		intptr_t current_chunk_ptr{};
		u64 current_chunk_offset{};
	};
}