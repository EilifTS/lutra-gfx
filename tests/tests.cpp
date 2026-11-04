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

TEST(Graphics, ComputeIntegerAtomics)
{
	lgx::GraphicsContext ctx("Test context");

	constexpr uint32_t group_count = 4;
	constexpr uint32_t thread_count = group_count * 64;

	const std::vector<int32_t> zeros(2, 0);
	lgx::Buffer buffer(ctx, zeros.size() * sizeof(int32_t), lgx::BufferType::StorageBuffer);

	const std::string cs_path = std::string(TESTS_SHADER_DIR) + "/AtomicCounter.cs.spv";
	lgx::ComputePipelineInfo pipeline_info{};
	pipeline_info.cs_name = cs_path.c_str();
	pipeline_info.AddStorageBuffer(0);
	lgx::ComputePipeline pipeline(ctx, pipeline_info);

	lgx::CommandBuffer cmd_buf(ctx);
	cmd_buf.ScheduleUpload(zeros.data(), zeros.size() * sizeof(int32_t), buffer);
	cmd_buf.Barrier(buffer, lgx::ResourceUsage::TransferDst, lgx::ResourceUsage::ComputeReadWrite);
	cmd_buf.BindPipeline(pipeline);
	cmd_buf.BindBuffer(buffer, 0);
	cmd_buf.Dispatch(group_count, 1, 1);
	cmd_buf.Barrier(buffer, lgx::ResourceUsage::ComputeReadWrite, lgx::ResourceUsage::TransferSrc);
	lgx::SubmitAndWait(ctx, cmd_buf);

	std::vector<int32_t> result(2, 0);
	lgx::Download(ctx, buffer, result.data(), result.size() * sizeof(int32_t));

	/* Every thread adds one to the first counter and its (one based) index to the second */
	EXPECT_EQ(result[0], static_cast<int32_t>(thread_count));
	EXPECT_EQ(result[1], static_cast<int32_t>(thread_count * (thread_count + 1) / 2));
}

TEST(Graphics, OptionalFeaturesAreOffByDefault)
{
	lgx::GraphicsContext ctx("Test context");
	EXPECT_FALSE(ctx.EnabledFeatures().shader_buffer_float_atomic_add);

	lgx::GraphicsContext ctx_no_request("Test context", lgx::GraphicsFeatures{});
	EXPECT_FALSE(ctx_no_request.EnabledFeatures().shader_buffer_float_atomic_add);
}

/* Optional feature, so this test is skipped on GPUs without it */
TEST(Graphics, ComputeFloatAtomicAdd)
{
	lgx::GraphicsFeatures features{};
	features.shader_buffer_float_atomic_add = true;
	lgx::GraphicsContext ctx("Test context", features);
	if (!ctx.EnabledFeatures().shader_buffer_float_atomic_add)
	{
		GTEST_SKIP() << "The GPU does not support atomic float add on storage buffers";
	}

	constexpr uint32_t group_count = 4;
	constexpr uint32_t thread_count = group_count * 64;

	const std::vector<float> zeros(2, 0.0f);
	lgx::Buffer buffer(ctx, zeros.size() * sizeof(float), lgx::BufferType::StorageBuffer);

	const std::string cs_path = std::string(TESTS_SHADER_DIR) + "/FloatAtomicAdd.cs.spv";
	lgx::ComputePipelineInfo pipeline_info{};
	pipeline_info.cs_name = cs_path.c_str();
	pipeline_info.AddStorageBuffer(0);
	lgx::ComputePipeline pipeline(ctx, pipeline_info);

	lgx::CommandBuffer cmd_buf(ctx);
	cmd_buf.ScheduleUpload(zeros.data(), zeros.size() * sizeof(float), buffer);
	cmd_buf.Barrier(buffer, lgx::ResourceUsage::TransferDst, lgx::ResourceUsage::ComputeReadWrite);
	cmd_buf.BindPipeline(pipeline);
	cmd_buf.BindBuffer(buffer, 0);
	cmd_buf.Dispatch(group_count, 1, 1);
	cmd_buf.Barrier(buffer, lgx::ResourceUsage::ComputeReadWrite, lgx::ResourceUsage::TransferSrc);
	lgx::SubmitAndWait(ctx, cmd_buf);

	std::vector<float> result(2, 0.0f);
	lgx::Download(ctx, buffer, result.data(), result.size() * sizeof(float));

	/* Every thread adds 0.5 to the first sum and its index to the second. All partial sums are
	   exact in a float, so the order the threads run in does not matter. */
	EXPECT_EQ(result[0], 0.5f * thread_count);
	EXPECT_EQ(result[1], static_cast<float>(thread_count * (thread_count - 1) / 2));
}

/* A simulation step needs a handful of dispatches, each with its own pipeline bind, so a frame
   can easily need more descriptor sets than fit in one descriptor pool. */
TEST(Graphics, ComputeManyDispatches)
{
	lgx::GraphicsContext ctx("Test context");

	constexpr uint32_t dispatch_count = 500;
	constexpr uint32_t thread_count = 64;

	const std::vector<int32_t> zeros(2, 0);
	lgx::Buffer buffer(ctx, zeros.size() * sizeof(int32_t), lgx::BufferType::StorageBuffer);

	const std::string cs_path = std::string(TESTS_SHADER_DIR) + "/AtomicCounter.cs.spv";
	lgx::ComputePipelineInfo pipeline_info{};
	pipeline_info.cs_name = cs_path.c_str();
	pipeline_info.AddStorageBuffer(0);
	lgx::ComputePipeline pipeline(ctx, pipeline_info);

	lgx::CommandBuffer cmd_buf(ctx);

	/* Record the same work twice, with a reset in between, to make sure the descriptor pools are reused */
	for (uint32_t round = 0; round < 2; round++)
	{
		cmd_buf.Reset();
		cmd_buf.ScheduleUpload(zeros.data(), zeros.size() * sizeof(int32_t), buffer);
		cmd_buf.Barrier(buffer, lgx::ResourceUsage::TransferDst, lgx::ResourceUsage::ComputeReadWrite);
		for (uint32_t i = 0; i < dispatch_count; i++)
		{
			cmd_buf.BindPipeline(pipeline);
			cmd_buf.BindBuffer(buffer, 0);
			cmd_buf.Dispatch(1, 1, 1);
			cmd_buf.Barrier(buffer, lgx::ResourceUsage::ComputeReadWrite, lgx::ResourceUsage::ComputeReadWrite);
		}
		cmd_buf.Barrier(buffer, lgx::ResourceUsage::ComputeReadWrite, lgx::ResourceUsage::TransferSrc);
		lgx::SubmitAndWait(ctx, cmd_buf);

		std::vector<int32_t> result(2, 0);
		lgx::Download(ctx, buffer, result.data(), result.size() * sizeof(int32_t));

		EXPECT_EQ(result[0], static_cast<int32_t>(dispatch_count * thread_count));
		EXPECT_EQ(result[1], static_cast<int32_t>(dispatch_count * thread_count * (thread_count + 1) / 2));
	}
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}