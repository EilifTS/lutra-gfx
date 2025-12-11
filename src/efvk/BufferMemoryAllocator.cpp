#include "BufferMemoryAllocator.h"

namespace efvk
{
	static constexpr vk::BufferUsageFlagBits buffer_usage = vk::BufferUsageFlagBits::eTransferSrc;
	static constexpr VmaAllocationCreateFlagBits vma_flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

	inline u64 AlignUpPow2(u64 value, u64 alignment)
	{
		assert(alignment != 0);
		assert((alignment & (alignment - 1)) == 0); /* Alignment has to be pow 2 */

		return ((value + (alignment - 1)) & ~(alignment - 1));
	}

	BufferMemoryAllocator::BufferMemoryAllocator(GraphicsContextInternal& ctx, u64 chunk_size)
		: ctx(&ctx), chunk_size(chunk_size)
	{
		chunks.push_back(BufferInternal(ctx, chunk_size, buffer_usage, vma_flags));
		current_chunk_offset = 0;
		first_chunk_ptr = chunks.back().Map(*ctx.device);
		current_chunk_ptr = reinterpret_cast<intptr_t>(first_chunk_ptr);
	}

	BufferMemoryAllocator::~BufferMemoryAllocator()
	{
		if (ctx != nullptr && chunks.size() > 0)
		{
			Reset(*ctx);
			chunks.back().Unmap(*ctx->device);
		}
	}

	BufferMemoryAllocation BufferMemoryAllocator::Alloc(GraphicsContextInternal& ctx, u64 size, u64 alignment)
	{
		/* Try to fit in current block */
		current_chunk_offset = AlignUpPow2(current_chunk_offset, alignment);
		const u64 remaining_chunk_capacity = chunks.back().Size() - current_chunk_offset;

		if (remaining_chunk_capacity < size)
		{
			/* Allocate a new block */
			const u64 new_block_size = std::max(chunk_size, size);
			chunks.push_back(BufferInternal(ctx, new_block_size, buffer_usage, vma_flags));
			current_chunk_offset = 0;
			current_chunk_ptr = reinterpret_cast<intptr_t>(chunks.back().Map(*ctx.device));
		}
		BufferMemoryAllocation allocation{
			.buffer = chunks.back().buffer.GetBuffer(),
			.vma_allocation = chunks.back().buffer.GetAllocation(),
			.ptr = reinterpret_cast<char*>(current_chunk_ptr + current_chunk_offset),
			.offset = current_chunk_offset,
			.size = size,
		};

		return allocation;
	}

	void BufferMemoryAllocator::Reset(GraphicsContextInternal& ctx)
	{
		/* Keep the first chunk around */
		for (u32 i = 1; i < static_cast<u32>(chunks.size()); i++)
		{
			chunks[i].Unmap(*ctx.device);
		}
		chunks.resize(1);
		
		current_chunk_ptr = reinterpret_cast<intptr_t>(first_chunk_ptr);
		current_chunk_offset = 0;
	}
}