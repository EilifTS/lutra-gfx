#pragma once
#include <lutra-gfx/CommonStructs.h>

#include "BufferInternal.h"
#include "GraphicsContextInternal.h"

namespace lgx
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
		BufferMemoryAllocator(GraphicsContextInternal& ctx, u64 chunk_size);
		BufferMemoryAllocator(const BufferMemoryAllocator&) = delete;
		BufferMemoryAllocator(BufferMemoryAllocator&&) = default;
		BufferMemoryAllocator& operator=(const BufferMemoryAllocator&) = delete;
		BufferMemoryAllocator& operator=(BufferMemoryAllocator&&) = default;
		~BufferMemoryAllocator();

		BufferMemoryAllocation Alloc(GraphicsContextInternal& ctx, u64 size, u64 alignment);
		void Reset(GraphicsContextInternal& ctx);

	private:
		GraphicsContextInternal* ctx{};

		void* first_chunk_ptr = nullptr;
		u64 chunk_size{};
		std::vector<BufferInternal> chunks{};
		intptr_t current_chunk_ptr{};
		u64 current_chunk_offset{};
	};
}