#include <efvk/SpriteBatch.h>

namespace efvk
{
	static constexpr u32 max_sprites_per_batch = 10000;

	SpriteBatch::SpriteBatch(GraphicsContext& ctx)
	{
		this->ctx = &ctx;

		sprite_buffer = Buffer(ctx, max_sprites_per_batch * sizeof(SpriteInternal), BufferType::StorageBuffer);

		ds_buffer = DepthStencilBuffer(ctx, 800, 600, DepthStencilFormat::D32);

		/* Create graphics pipeline */
		GraphicsPipelineInfo pipeline_info{};
		pipeline_info.vs_name = "shaders/SpriteBatchShader.vert.spv";
		pipeline_info.ps_name = "shaders/SpriteBatchShader.frag.spv";
		pipeline_info.ds_info.depth_enabled = true;
		pipeline_info.ds_info.ds_format = DepthStencilFormat::D32;
		pipeline_info.AddStorageBuffer(0, GraphicsPipelineInfo::Binding::Stage::Vertex);
		pipeline_info.AddImmutableSampler(1, SamplerType::LinearClamp, GraphicsPipelineInfo::Binding::Stage::Fragment);
		pipeline_info.AddTextures(2, max_texture_count, GraphicsPipelineInfo::Binding::Stage::Fragment);
		pipeline = GraphicsPipeline(ctx, pipeline_info);

		Image placeholder_img(1, 1);
		placeholder_img.SetData(0, 255);
		placeholder_img.SetData(1, 0);
		placeholder_img.SetData(2, 0);
		placeholder_img.SetData(3, 255);
		placeholder_texture = Texture(ctx, placeholder_img);
	}

	void SpriteBatch::End(FrameManager& frame_manager)
	{
		CommandBuffer& cmd_buf = frame_manager.GetCurrentCommandBuffer();

		if (sprite_list.size() > 0)
		{
			/* Upload sprite data */
			cmd_buf.ScheduleUpload(sprite_list.data(), sprite_list.size() * sizeof(SpriteInternal), sprite_buffer);

			cmd_buf.BeginRendering(frame_manager.GetCurrentTetureView(), ds_buffer.DefaultView(), frame_manager.FrameWidth(), frame_manager.FrameHeight(), true);

			cmd_buf.BindPipeline(pipeline);
			cmd_buf.BindBuffer(sprite_buffer, 0);
			cmd_buf.BindTextures(texture_view_array, 2);

			cmd_buf.Draw(6, static_cast<u32>(sprite_list.size()));
			
			cmd_buf.EndRendering();
		}
	}
}