#include <gtest/gtest.h>
#include <lutra-gfx/GraphicsContext.h>
#include <lutra-gfx/Buffer.h>
#include <lutra-gfx/CommandBuffer.h>
#include <lutra-gfx/RenderTarget.h>
#include <lutra-gfx/ComputePipeline.h>
#include <vector>
#include <cstdint>
#include <string>

TEST(Graphics, CreateContext)
{
	lgx::GraphicsContext ctx("Test context");
	(void)ctx;
}

TEST(Graphics, BufferUploadDownloadRoundTrip)
{
	lgx::GraphicsContext ctx("Test context");

	constexpr uint32_t element_count = 64;
	std::vector<uint32_t> src_data(element_count);
	for (uint32_t i = 0; i < element_count; i++)
	{
		src_data[i] = i * 7 + 1;
	}

	lgx::Buffer buffer(ctx, src_data.size() * sizeof(uint32_t), lgx::BufferType::StorageBuffer);

	lgx::CommandBuffer cmd_buf(ctx);
	cmd_buf.ScheduleUpload(src_data.data(), src_data.size() * sizeof(uint32_t), buffer);
	lgx::SubmitAndWait(ctx, cmd_buf);

	std::vector<uint32_t> readback_data(element_count, 0);
	lgx::Download(ctx, buffer, readback_data.data(), readback_data.size() * sizeof(uint32_t));

	EXPECT_EQ(src_data, readback_data);
}

TEST(Graphics, RenderTargetClearAndDownload)
{
	lgx::GraphicsContext ctx("Test context");

	constexpr uint32_t width = 8;
	constexpr uint32_t height = 8;

	lgx::RenderTarget rt(ctx, width, height);
	EXPECT_EQ(rt.Width(), width);
	EXPECT_EQ(rt.Height(), height);

	lgx::CommandBuffer cmd_buf(ctx);
	cmd_buf.BeginRendering(rt.DefaultView(), nullptr, width, height, true);
	cmd_buf.EndRendering();
	cmd_buf.Barrier(rt, lgx::ResourceUsage::ColorAttachment, lgx::ResourceUsage::TransferSrc);
	lgx::SubmitAndWait(ctx, cmd_buf);

	std::vector<uint8_t> pixels(static_cast<size_t>(width) * height * 4, 0xAA);
	lgx::Download(ctx, rt, pixels.data());

	/* BeginRendering's clear color is hardcoded to (0,0,0,1) -> RGBA8 (0,0,0,255) */
	for (uint32_t i = 0; i < width * height; i++)
	{
		EXPECT_EQ(pixels[i * 4 + 0], 0);
		EXPECT_EQ(pixels[i * 4 + 1], 0);
		EXPECT_EQ(pixels[i * 4 + 2], 0);
		EXPECT_EQ(pixels[i * 4 + 3], 255);
	}
}

TEST(Graphics, ComputeDoubleBuffer)
{
	lgx::GraphicsContext ctx("Test context");

	constexpr uint32_t element_count = 16;
	std::vector<uint32_t> src_data(element_count);
	for (uint32_t i = 0; i < element_count; i++)
	{
		src_data[i] = i + 1;
	}

	lgx::Buffer buffer(ctx, src_data.size() * sizeof(uint32_t), lgx::BufferType::StorageBuffer);

	lgx::CommandBuffer upload_cmd_buf(ctx);
	upload_cmd_buf.ScheduleUpload(src_data.data(), src_data.size() * sizeof(uint32_t), buffer);
	lgx::SubmitAndWait(ctx, upload_cmd_buf);

	const std::string cs_path = std::string(TESTS_SHADER_DIR) + "/DoubleBuffer.cs.spv";
	lgx::ComputePipelineInfo pipeline_info{};
	pipeline_info.cs_name = cs_path.c_str();
	pipeline_info.AddStorageBuffer(0);
	lgx::ComputePipeline pipeline(ctx, pipeline_info);

	lgx::CommandBuffer compute_cmd_buf(ctx);
	compute_cmd_buf.BindPipeline(pipeline);
	compute_cmd_buf.BindBuffer(buffer, 0);
	compute_cmd_buf.Dispatch(element_count, 1, 1);
	compute_cmd_buf.Barrier(buffer, lgx::ResourceUsage::ComputeWrite, lgx::ResourceUsage::TransferSrc);
	lgx::SubmitAndWait(ctx, compute_cmd_buf);

	std::vector<uint32_t> readback_data(element_count, 0);
	lgx::Download(ctx, buffer, readback_data.data(), readback_data.size() * sizeof(uint32_t));

	for (uint32_t i = 0; i < element_count; i++)
	{
		EXPECT_EQ(readback_data[i], src_data[i] * 2);
	}
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}