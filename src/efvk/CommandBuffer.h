#pragma once
#include <efvk/GraphicsContext.h>
#include <efvk/Texture.h>
#include "GraphicsContextImpl.h"
#include "BufferMemoryAllocator.h"
#include "DescriptorAllocator.h"
#include "DescriptorWriteCache.h"

namespace efvk
{
	class GraphicsPipeline;

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

		void BeginRendering(vk::ImageView image_view, u32 width, u32 height);
		void EndRendering();

		vk::DescriptorSet AllocateDescriptorSet(vk::DescriptorSetLayout layout) { return descriptor_allocator.Alloc(layout); };
		void BindPipeline(GraphicsPipeline& pipeline);
		void BindBuffer(Buffer& buffer, u32 binding);
		void BindTexture(Texture& texture, u32 binding);
		void BindTextures(std::span<Texture*> textures, u32 binding);

		void ScheduleUpload(const void* src_ptr, u64 size, Buffer& dst_buffer);
		void ScheduleUpload(const void* src_ptr, Texture& dst_texture);

		void Draw(u32 vertex_count, u32 instance_count);

		void Reset();

	private:
		GraphicsContext::Impl* ctx{};

		BufferMemoryAllocator buffer_memory_allocator{};
		DescriptorAllocator descriptor_allocator{};
		DescriptorWriteCache descriptor_write_cache{};

		GraphicsPipeline* bound_pipeline{};
	};

	void SubmitAndWait(GraphicsContext::Impl& ctx, CommandBuffer& cmd_buf);
}