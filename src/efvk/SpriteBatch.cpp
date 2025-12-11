#include <efvk/SpriteBatch.h>

#include "FrameManagerImpl.h"
#include "VulkanHPP.h"
#include "GraphicsPipeline.h"
#include "internal/GraphicsContextInternal.h"

namespace efvk
{
	static constexpr u32 max_sprites_per_batch = 10000;

	struct SpriteBatch::Impl
	{
		vk::Device dev{};
		GraphicsPipeline pipeline{};
	};

	SpriteBatch::SpriteBatch(GraphicsContext& ctx)
	{
		pimpl = std::make_unique<Impl>();

		pimpl->dev = *ctx.internal->device;

		sprite_buffer = Buffer(ctx, max_sprites_per_batch * sizeof(SpriteInternal), BufferType::StorageBuffer);

		/* Create graphics pipeline */
		GraphicsPipelineInfo pipeline_info{};
		pipeline_info.vs_name = "shaders/SpriteBatchShader.vert.spv";
		pipeline_info.ps_name = "shaders/SpriteBatchShader.frag.spv";
		pipeline_info.AddStorageBuffer(0, GraphicsPipelineInfo::Binding::Stage::Vertex);
		pipeline_info.AddImmutableSampler(1, SamplerType::LinearClamp, GraphicsPipelineInfo::Binding::Stage::Fragment);
		pipeline_info.AddTextures(2, max_texture_count, GraphicsPipelineInfo::Binding::Stage::Fragment);

		pimpl->pipeline = GraphicsPipeline(*ctx.internal->device, pipeline_info);

		Image placeholder_img(1, 1);
		placeholder_img.SetData(0, 255);
		placeholder_img.SetData(1, 0);
		placeholder_img.SetData(2, 0);
		placeholder_img.SetData(3, 255);
		placeholder_texture = Texture(ctx, placeholder_img);
	}

	SpriteBatch::~SpriteBatch()
	{

	}

	SpriteBatch::SpriteBatch(SpriteBatch&&) = default;
	SpriteBatch& SpriteBatch::operator=(SpriteBatch&&) = default;

	void SpriteBatch::End(FrameManager& frame_manager)
	{
		CommandBuffer& cmd_buf = frame_manager.pimpl->GetCurrentCommandBuffer();

		if (sprite_list.size() > 0)
		{
			/* Upload sprite data */
			cmd_buf.ScheduleUpload(sprite_list.data(), sprite_list.size() * sizeof(SpriteInternal), sprite_buffer);

			cmd_buf.BeginRendering(frame_manager.pimpl->GetCurrentImageView(), frame_manager.pimpl->window_width, frame_manager.pimpl->window_height);

			cmd_buf.BindPipeline(pimpl->pipeline);
			cmd_buf.BindBuffer(sprite_buffer, 0);
			cmd_buf.BindTextures(texture_array, 2);

			cmd_buf.Draw(6, static_cast<u32>(sprite_list.size()));
			
			cmd_buf.EndRendering();
		}
	}
}