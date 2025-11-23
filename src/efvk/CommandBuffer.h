#pragma once
#include <efvk/GraphicsContext.h>
#include <efvk/Texture.h>
#include "GraphicsContextImpl.h"
#include "BufferMemoryAllocator.h"
#include "DescriptorAllocator.h"

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
		void ScheduleUpload(const void* src_ptr, Texture& dst_texture);
		vk::DescriptorSet AllocateDescriptorSet(vk::DescriptorSetLayout layout) { return descriptor_allocator.Alloc(layout); };
		void Reset();

	private:
		GraphicsContext::Impl* ctx{};

		BufferMemoryAllocator buffer_memory_allocator{};
		DescriptorAllocator descriptor_allocator{};
	};

	void SubmitAndWait(GraphicsContext::Impl& ctx, CommandBuffer& cmd_buf);
}