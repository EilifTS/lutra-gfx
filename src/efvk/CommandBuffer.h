#pragma once
#include <efvk/GraphicsContext.h>
#include "GraphicsContextImpl.h"
#include "BufferMemoryAllocator.h"

namespace efvk
{
	class CommandBuffer
	{
	public:
		CommandBuffer() {};
		CommandBuffer(GraphicsContext::Impl& ctx);

		CommandBuffer(const CommandBuffer&) = delete;
		CommandBuffer(CommandBuffer&&) = default;
		CommandBuffer& operator=(const CommandBuffer&) = delete;
		CommandBuffer& operator=(CommandBuffer&&) = default;

		vk::UniqueCommandBuffer cmd_buf{};

		void ScheduleUpload(const void* src_ptr, u64 size, Buffer& dst_buffer);
		void Reset();

	private:
		GraphicsContext::Impl* ctx{};

		BufferMemoryAllocator buffer_memory_allocator{};
	};
}