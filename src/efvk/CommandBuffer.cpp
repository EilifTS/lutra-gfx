#include <efvk/CommandBuffer.h>
#include <efvk/Buffer.h>
#include <efvk/Texture.h>
#include "internal/GraphicsContextInternal.h"
#include "internal/CommandBufferInternal.h"
#include <vector>

namespace efvk
{
	CommandBuffer::CommandBuffer() {}
	CommandBuffer::CommandBuffer(GraphicsContext& ctx)
	{
		internal = std::make_unique<CommandBufferInternal>(*ctx.internal);
	}

	CommandBuffer::~CommandBuffer() {}

	CommandBuffer::CommandBuffer(CommandBuffer&&) = default;
	CommandBuffer& CommandBuffer::operator=(CommandBuffer&&) = default;

	void CommandBuffer::BeginRendering(TextureView color_view, TextureView ds_view, u32 width, u32 height, bool clear)
	{
		internal->BeginRendering(*reinterpret_cast<vk::ImageView*>(&color_view), *reinterpret_cast<vk::ImageView*>(&ds_view), width, height, clear);
	}

	void CommandBuffer::EndRendering() { internal->EndRendering(); }

	void CommandBuffer::BindPipeline(GraphicsPipeline& pipeline) { internal->BindPipeline(*pipeline.internal); };
	void CommandBuffer::BindBuffer(Buffer& buffer, u32 binding) { internal->BindBuffer(*buffer.internal, binding); };
	void CommandBuffer::BindTexture(TextureView view, u32 binding) { internal->BindTexture(*reinterpret_cast<vk::ImageView*>(&view), binding); };
	void CommandBuffer::BindTextures(std::span<TextureView> views, u32 binding)
	{
		std::span<vk::ImageView>* vk_views = reinterpret_cast<std::span<vk::ImageView>*>(&views);
		internal->BindTextures(*vk_views, binding);
	};

	void CommandBuffer::ScheduleUpload(const void* src_ptr, u64 size, Buffer& dst_buffer) { internal->ScheduleUpload(src_ptr, size, *dst_buffer.internal); };
	void CommandBuffer::ScheduleUpload(const void* src_ptr, Texture& dst_texture) { internal->ScheduleUpload(src_ptr, *dst_texture.internal); };

	void CommandBuffer::Draw(u32 vertex_count, u32 instance_count) { internal->Draw(vertex_count, instance_count); };

	void CommandBuffer::Reset() { internal->Reset(); };

	void SubmitAndWait(GraphicsContext& ctx, CommandBuffer& cmd_buf)
	{
		SubmitAndWaitInternal(*ctx.internal, *cmd_buf.internal);
	}
}