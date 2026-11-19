#pragma once
#include <memory>
#include <span>
#include "CommonStructs.h"
#include "Texture.h"

namespace lgx
{
	class CommandBufferInternal;

	class Buffer;
	class GraphicsContext;
	class GraphicsPipeline;
	class ComputePipeline;
	class DepthStencilBuffer;
	class RenderTarget;
	class Texture3D;

	class CommandBuffer
	{
	public:
		CommandBuffer();
		CommandBuffer(GraphicsContext& ctx);
		~CommandBuffer();

		CommandBuffer(const CommandBuffer&) = delete;
		CommandBuffer(CommandBuffer&&);
		CommandBuffer& operator=(const CommandBuffer&) = delete;
		CommandBuffer& operator=(CommandBuffer&&);

		void BeginRendering(TextureView color_view, TextureView ds_view, u32 width, u32 height, bool clear);
		void EndRendering();

		void BindPipeline(GraphicsPipeline& pipeline);
		void BindPipeline(ComputePipeline& pipeline);
		void BindBuffer(Buffer& buffer, u32 binding);

		/* Binds a part of a buffer, so one buffer can hold many things. The offset has to be a multiple of
		   GraphicsContext::MinUniformBufferOffsetAlignment() for a uniform buffer and of
		   MinStorageBufferOffsetAlignment() for a storage buffer. A shader sees the part as the whole buffer. */
		void BindBuffer(Buffer& buffer, u32 binding, u64 offset, u64 size = whole_size);
		void BindTexture(TextureView texture, u32 binding);
		void BindTextures(std::span<TextureView> textures, u32 binding);

		/* Binds a texture/render target for a compute shader to write via imageStore. The
		   resource must have been created with storage-image support (RenderTarget already
		   is; Texture is not, since it's meant to hold data uploaded from the CPU). */
		void BindStorageImage(TextureView texture, u32 binding);

		/* Copies size bytes to the buffer, starting dst_offset bytes into it */
		void ScheduleUpload(const void* src_ptr, u64 size, Buffer& dst_buffer, u64 dst_offset = 0);
		void ScheduleUpload(const void* src_ptr, Texture& dst_texture);

		/* Uploads the texels of one mip, tightly packed with x first, then y, then z.
		   The size is that of the mip in TextureFormatSize(format) bytes per texel. */
		void ScheduleUpload(const void* src_ptr, Texture3D& dst_texture, u32 mip);

		/* Copies between two buffers on the GPU, which can be the same buffer if the ranges do not overlap.
		   Use Barrier with TransferSrc and TransferDst to wait for what wrote the source and what reads the destination. */
		void CopyBuffer(Buffer& src_buffer, Buffer& dst_buffer, u64 size, u64 src_offset = 0, u64 dst_offset = 0);

		/* Sets every 32 bit word of a range of a buffer to value. The offset and size have to be a multiple of four,
		   except that a size of whole_size takes everything to the end, rounded down to a multiple of four. */
		void FillBuffer(Buffer& buffer, u32 value, u64 offset = 0, u64 size = whole_size);

		/* Sets the push constants of the bound pipeline, small values that the next draws and dispatches read without a
		   buffer. Meant for what changes from one dispatch to the next. Call it after BindPipeline, which resets them.
		   The offset and size are multiples of four and have to fit the push_constant_size of the pipeline. */
		void PushConstants(const void* data, u32 size, u32 offset = 0);

		/* Synchronizes access to a resource between what it was just used for and what it's
		   about to be used for - e.g. Barrier(rt, ResourceUsage::ColorAttachment,
		   ResourceUsage::ShaderRead) after rendering into a texture and before sampling it. */
		void Barrier(Texture& texture, ResourceUsage prev_usage, ResourceUsage next_usage);
		void Barrier(DepthStencilBuffer& depth_stencil_buffer, ResourceUsage prev_usage, ResourceUsage next_usage);
		void Barrier(RenderTarget& render_target, ResourceUsage prev_usage, ResourceUsage next_usage);
		/* Covers every mip */
		void Barrier(Texture3D& texture, ResourceUsage prev_usage, ResourceUsage next_usage);
		void Barrier(Buffer& buffer, ResourceUsage prev_usage, ResourceUsage next_usage);

		void Draw(u32 vertex_count, u32 instance_count);
		void Dispatch(u32 x, u32 y, u32 z);

		void Reset();

		std::unique_ptr<CommandBufferInternal> internal{};
	};

	void SubmitAndWait(GraphicsContext& ctx, CommandBuffer& cmd_buf);

	/* Blocking GPU -> CPU readback, handy for tests/tools: submits its own one-off command
	   buffer and waits for it, then copies the result into dst_ptr. Not meant for per-frame
	   use - for that, schedule your own copy through a persistent CommandBuffer instead.
	   dst_ptr must point at a buffer at least `size` bytes for the Buffer overload, or at
	   least Width() * Height() * 4 bytes for the Texture overload. */
	void Download(GraphicsContext& ctx, Buffer& src_buffer, void* dst_ptr, u64 size, u64 src_offset = 0);
	void Download(GraphicsContext& ctx, Texture& src_texture, void* dst_ptr);
	void Download(GraphicsContext& ctx, RenderTarget& src_render_target, void* dst_ptr);
	/* Reads one mip, tightly packed like ScheduleUpload takes it, so dst_ptr has to fit the whole mip */
	void Download(GraphicsContext& ctx, Texture3D& src_texture, void* dst_ptr, u32 mip);
}