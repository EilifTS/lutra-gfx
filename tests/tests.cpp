#include <gtest/gtest.h>
#include <lutra-gfx/GraphicsContext.h>
#include <lutra-gfx/Buffer.h>
#include <lutra-gfx/CommandBuffer.h>
#include <lutra-gfx/RenderTarget.h>
#include <lutra-gfx/ComputePipeline.h>
#include <lutra-gfx/Texture3D.h>
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

TEST(Texture3D, MipHelpers_CountDownToASingleTexel)
{
	EXPECT_EQ(lgx::CalculateMipCount(8, 4, 2), 4u);
	EXPECT_EQ(lgx::CalculateMipCount(1, 1, 1), 1u);
	EXPECT_EQ(lgx::CalculateMipCount(128, 128, 1), 8u);
	EXPECT_EQ(lgx::CalculateMipSize(8, 1), 4u);
	EXPECT_EQ(lgx::CalculateMipSize(2, 3), 1u);
}

TEST(Texture3D, UploadDownloadRoundTrip_EveryMip)
{
	lgx::GraphicsContext ctx("Test context");

	lgx::Texture3DInfo info{};
	info.width = 8;
	info.height = 4;
	info.depth = 2;
	info.format = lgx::TextureFormat::RGBA8;
	info.mip_count = lgx::CalculateMipCount(info.width, info.height, info.depth);
	lgx::Texture3D texture(ctx, info);
	EXPECT_EQ(texture.Width(), 8u);
	EXPECT_EQ(texture.Height(), 4u);
	EXPECT_EQ(texture.Depth(), 2u);
	EXPECT_EQ(texture.MipCount(), 4u);

	std::vector<std::vector<uint8_t>> mips;
	lgx::CommandBuffer cmd_buf(ctx);
	for (uint32_t mip = 0; mip < texture.MipCount(); mip++)
	{
		const uint32_t texel_count = lgx::CalculateMipSize(info.width, mip) * lgx::CalculateMipSize(info.height, mip) * lgx::CalculateMipSize(info.depth, mip);
		std::vector<uint8_t> data(texel_count * 4);
		for (size_t i = 0; i < data.size(); i++)
		{
			data[i] = static_cast<uint8_t>(mip * 50 + i);
		}
		cmd_buf.ScheduleUpload(data.data(), texture, mip);
		mips.push_back(std::move(data));
	}
	lgx::SubmitAndWait(ctx, cmd_buf);

	for (uint32_t mip = 0; mip < texture.MipCount(); mip++)
	{
		std::vector<uint8_t> readback(mips[mip].size(), 0);
		lgx::Download(ctx, texture, readback.data(), mip);
		EXPECT_EQ(readback, mips[mip]) << "mip " << mip;
	}
}

TEST(Texture3D, ComputeStorageWrite_ToOneMip_LeavesTheOthersAlone)
{
	lgx::GraphicsContext ctx("Test context");

	lgx::Texture3DInfo info{};
	info.width = 8;
	info.height = 4;
	info.depth = 4;
	info.format = lgx::TextureFormat::R32Float;
	info.mip_count = lgx::CalculateMipCount(info.width, info.height, info.depth);
	info.storage = true;
	lgx::Texture3D texture(ctx, info);

	const std::string cs_path = std::string(TESTS_SHADER_DIR) + "/Texture3DFill.cs.spv";
	lgx::ComputePipelineInfo pipeline_info{};
	pipeline_info.cs_name = cs_path.c_str();
	pipeline_info.AddStorageImage(0);
	lgx::ComputePipeline pipeline(ctx, pipeline_info);

	const std::vector<float> mip0_data(8 * 4 * 4, -1.0f);
	lgx::CommandBuffer cmd_buf(ctx);
	cmd_buf.ScheduleUpload(mip0_data.data(), texture, 0);
	cmd_buf.Barrier(texture, lgx::ResourceUsage::TransferDst, lgx::ResourceUsage::ComputeWrite);
	cmd_buf.BindPipeline(pipeline);
	cmd_buf.BindStorageImage(texture.MipView(1), 0);
	cmd_buf.Dispatch(1, 1, 1);
	cmd_buf.Barrier(texture, lgx::ResourceUsage::ComputeWrite, lgx::ResourceUsage::TransferSrc);
	lgx::SubmitAndWait(ctx, cmd_buf);

	std::vector<float> mip1(4 * 2 * 2, 0.0f);
	lgx::Download(ctx, texture, mip1.data(), 1);
	for (uint32_t z = 0; z < 2; z++)
	{
		for (uint32_t y = 0; y < 2; y++)
		{
			for (uint32_t x = 0; x < 4; x++)
			{
				EXPECT_EQ(mip1[(z * 2 + y) * 4 + x], static_cast<float>(x + 10 * y + 100 * z));
			}
		}
	}

	std::vector<float> mip0(mip0_data.size(), 0.0f);
	lgx::Download(ctx, texture, mip0.data(), 0);
	EXPECT_EQ(mip0, mip0_data);
}

namespace
{
	/* Samples the texture, at the position and mip of every query, in a compute shader with a linear clamp sampler */
	std::vector<float> SampleTexture(lgx::GraphicsContext& ctx, lgx::Texture3D& texture, const std::vector<float>& queries_xyzw)
	{
		const uint32_t query_count = static_cast<uint32_t>(queries_xyzw.size() / 4);
		lgx::Buffer query_buffer(ctx, queries_xyzw.size() * sizeof(float), lgx::BufferType::StorageBuffer);
		lgx::Buffer result_buffer(ctx, query_count * sizeof(float), lgx::BufferType::StorageBuffer);

		const std::string cs_path = std::string(TESTS_SHADER_DIR) + "/Texture3DSample.cs.spv";
		lgx::ComputePipelineInfo pipeline_info{};
		pipeline_info.cs_name = cs_path.c_str();
		pipeline_info.AddTexture(0);
		pipeline_info.AddImmutableSampler(1, lgx::SamplerType::LinearClamp);
		pipeline_info.AddStorageBuffer(2);
		pipeline_info.AddStorageBuffer(3);
		lgx::ComputePipeline pipeline(ctx, pipeline_info);

		lgx::CommandBuffer cmd_buf(ctx);
		cmd_buf.ScheduleUpload(queries_xyzw.data(), queries_xyzw.size() * sizeof(float), query_buffer);
		cmd_buf.Barrier(query_buffer, lgx::ResourceUsage::TransferDst, lgx::ResourceUsage::ShaderRead);
		cmd_buf.Barrier(texture, lgx::ResourceUsage::TransferDst, lgx::ResourceUsage::ShaderRead);
		cmd_buf.BindPipeline(pipeline);
		cmd_buf.BindTexture(texture.DefaultView(), 0);
		cmd_buf.BindBuffer(query_buffer, 2);
		cmd_buf.BindBuffer(result_buffer, 3);
		cmd_buf.Dispatch(query_count, 1, 1);
		cmd_buf.Barrier(result_buffer, lgx::ResourceUsage::ComputeWrite, lgx::ResourceUsage::TransferSrc);
		lgx::SubmitAndWait(ctx, cmd_buf);

		std::vector<float> results(query_count, 0.0f);
		lgx::Download(ctx, result_buffer, results.data(), results.size() * sizeof(float));
		return results;
	}

	constexpr float snorm_step = 1.0f / 127.0f;
}

TEST(Texture3D, Sampling_InterpolatesBetweenTexelsAndClampsAtTheEdge)
{
	lgx::GraphicsContext ctx("Test context");

	lgx::Texture3DInfo info{};
	info.width = 2;
	info.height = 2;
	info.depth = 2;
	info.format = lgx::TextureFormat::R8Snorm;
	lgx::Texture3D texture(ctx, info);

	/* The texel at (x, y, z) is 16 * (x + 2 y + 4 z) */
	std::vector<int8_t> texels(8);
	for (int8_t i = 0; i < 8; i++)
	{
		texels[i] = static_cast<int8_t>(i * 16);
	}
	lgx::CommandBuffer upload_cmd_buf(ctx);
	upload_cmd_buf.ScheduleUpload(texels.data(), texture, 0);
	lgx::SubmitAndWait(ctx, upload_cmd_buf);

	const std::vector<float> queries = {
		0.25f, 0.25f, 0.25f, 0.0f, /* the middle of the first texel */
		0.75f, 0.75f, 0.75f, 0.0f, /* the middle of the last texel */
		0.50f, 0.50f, 0.50f, 0.0f, /* between all of them */
		0.50f, 0.25f, 0.25f, 0.0f, /* between the first and the second */
		0.00f, 0.00f, 0.00f, 0.0f, /* outside, in front of the first texel */
		1.50f, 1.50f, 1.50f, 0.0f, /* outside, behind the last texel */
	};
	const std::vector<float> results = SampleTexture(ctx, texture, queries);

	constexpr float tolerance = 0.005f;
	EXPECT_NEAR(results[0], 0.0f * snorm_step, tolerance);
	EXPECT_NEAR(results[1], 112.0f * snorm_step, tolerance);
	EXPECT_NEAR(results[2], 56.0f * snorm_step, tolerance);
	EXPECT_NEAR(results[3], 8.0f * snorm_step, tolerance);
	EXPECT_NEAR(results[4], 0.0f * snorm_step, tolerance);
	EXPECT_NEAR(results[5], 112.0f * snorm_step, tolerance);
}

TEST(Texture3D, Sampling_PicksTheMipOfTheLevel)
{
	lgx::GraphicsContext ctx("Test context");

	lgx::Texture3DInfo info{};
	info.width = 4;
	info.height = 4;
	info.depth = 4;
	info.format = lgx::TextureFormat::R8Snorm;
	info.mip_count = 3;
	lgx::Texture3D texture(ctx, info);

	lgx::CommandBuffer upload_cmd_buf(ctx);
	std::vector<std::vector<int8_t>> mips;
	for (uint32_t mip = 0; mip < info.mip_count; mip++)
	{
		const uint32_t side = lgx::CalculateMipSize(info.width, mip);
		mips.emplace_back(side * side * side, static_cast<int8_t>((mip + 1) * 20));
		upload_cmd_buf.ScheduleUpload(mips.back().data(), texture, mip);
	}
	lgx::SubmitAndWait(ctx, upload_cmd_buf);

	const std::vector<float> queries = {
		0.3f, 0.6f, 0.9f, 0.0f,
		0.3f, 0.6f, 0.9f, 1.0f,
		0.3f, 0.6f, 0.9f, 2.0f,
	};
	const std::vector<float> results = SampleTexture(ctx, texture, queries);

	EXPECT_NEAR(results[0], 20.0f * snorm_step, 0.005f);
	EXPECT_NEAR(results[1], 40.0f * snorm_step, 0.005f);
	EXPECT_NEAR(results[2], 60.0f * snorm_step, 0.005f);
}

TEST(Texture3D, SnormStorageWrite_ThenSample_ReadsWhatTheShaderWrote)
{
	lgx::GraphicsContext ctx("Test context");

	lgx::Texture3DInfo info{};
	info.width = 4;
	info.height = 4;
	info.depth = 4;
	info.format = lgx::TextureFormat::R8Snorm;
	info.storage = true;
	lgx::Texture3D texture(ctx, info);

	const std::string cs_path = std::string(TESTS_SHADER_DIR) + "/Texture3DFillSnorm.cs.spv";
	lgx::ComputePipelineInfo pipeline_info{};
	pipeline_info.cs_name = cs_path.c_str();
	pipeline_info.AddStorageImage(0);
	lgx::ComputePipeline pipeline(ctx, pipeline_info);

	lgx::CommandBuffer cmd_buf(ctx);
	cmd_buf.BindPipeline(pipeline);
	cmd_buf.BindStorageImage(texture.MipView(0), 0);
	cmd_buf.Dispatch(1, 1, 1);
	cmd_buf.Barrier(texture, lgx::ResourceUsage::ComputeWrite, lgx::ResourceUsage::TransferSrc);
	lgx::SubmitAndWait(ctx, cmd_buf);

	/* The shader writes -1, -1/3, 1/3 and 1 along x */
	std::vector<int8_t> texels(4 * 4 * 4, 0);
	lgx::Download(ctx, texture, texels.data(), 0);
	const int8_t expected_row[4] = { -127, -42, 42, 127 };
	for (uint32_t i = 0; i < texels.size(); i++)
	{
		EXPECT_EQ(texels[i], expected_row[i % 4]) << "texel " << i;
	}

	const std::vector<float> queries = {
		0.125f, 0.5f, 0.5f, 0.0f,
		0.375f, 0.5f, 0.5f, 0.0f,
		0.625f, 0.5f, 0.5f, 0.0f,
		0.875f, 0.5f, 0.5f, 0.0f,
	};
	const std::vector<float> results = SampleTexture(ctx, texture, queries);
	for (uint32_t i = 0; i < 4; i++)
	{
		EXPECT_NEAR(results[i], static_cast<float>(expected_row[i]) * snorm_step, 0.005f);
	}
}

namespace
{
	std::vector<uint32_t> DownloadWords(lgx::GraphicsContext& ctx, lgx::Buffer& buffer, uint32_t word_count, uint64_t offset = 0)
	{
		std::vector<uint32_t> words(word_count, 0);
		lgx::Download(ctx, buffer, words.data(), words.size() * sizeof(uint32_t), offset);
		return words;
	}

	/* Makes a storage buffer with these words in it, ready to be used by anything */
	lgx::Buffer CreateWordBuffer(lgx::GraphicsContext& ctx, const std::vector<uint32_t>& words, lgx::BufferType type = lgx::BufferType::StorageBuffer)
	{
		lgx::Buffer buffer(ctx, words.size() * sizeof(uint32_t), type);
		lgx::CommandBuffer cmd_buf(ctx);
		cmd_buf.ScheduleUpload(words.data(), words.size() * sizeof(uint32_t), buffer);
		lgx::SubmitAndWait(ctx, cmd_buf);
		return buffer;
	}
}

TEST(Buffer, ContextLimits_AreUsableForOffsetsAndPushConstants)
{
	lgx::GraphicsContext ctx("Test context");

	const uint32_t uniform_alignment = ctx.MinUniformBufferOffsetAlignment();
	const uint32_t storage_alignment = ctx.MinStorageBufferOffsetAlignment();
	EXPECT_GT(uniform_alignment, 0u);
	EXPECT_EQ(uniform_alignment & (uniform_alignment - 1), 0u);
	EXPECT_GT(storage_alignment, 0u);
	EXPECT_EQ(storage_alignment & (storage_alignment - 1), 0u);
	EXPECT_GE(ctx.MaxPushConstantsSize(), 128u);
}

TEST(Buffer, ScheduleUpload_AtAnOffset_ChangesOnlyThatRange)
{
	lgx::GraphicsContext ctx("Test context");

	lgx::Buffer buffer = CreateWordBuffer(ctx, std::vector<uint32_t>(8, 0));

	const std::vector<uint32_t> part = { 1, 2, 3 };
	lgx::CommandBuffer cmd_buf(ctx);
	cmd_buf.ScheduleUpload(part.data(), part.size() * sizeof(uint32_t), buffer, 2 * sizeof(uint32_t));
	lgx::SubmitAndWait(ctx, cmd_buf);

	EXPECT_EQ(DownloadWords(ctx, buffer, 8), (std::vector<uint32_t>{ 0, 0, 1, 2, 3, 0, 0, 0 }));
}

TEST(Buffer, Download_FromAnOffset_ReadsThatRange)
{
	lgx::GraphicsContext ctx("Test context");

	lgx::Buffer buffer = CreateWordBuffer(ctx, { 10, 11, 12, 13, 14, 15 });

	EXPECT_EQ(DownloadWords(ctx, buffer, 3, 2 * sizeof(uint32_t)), (std::vector<uint32_t>{ 12, 13, 14 }));
}

TEST(Buffer, CopyBuffer_WithOffsets_CopiesTheRange)
{
	lgx::GraphicsContext ctx("Test context");

	lgx::Buffer src = CreateWordBuffer(ctx, { 0, 1, 2, 3, 4, 5, 6, 7 });
	lgx::Buffer dst = CreateWordBuffer(ctx, std::vector<uint32_t>(8, 100));

	lgx::CommandBuffer cmd_buf(ctx);
	cmd_buf.CopyBuffer(src, dst, 3 * sizeof(uint32_t), 2 * sizeof(uint32_t), 4 * sizeof(uint32_t));
	lgx::SubmitAndWait(ctx, cmd_buf);

	EXPECT_EQ(DownloadWords(ctx, dst, 8), (std::vector<uint32_t>{ 100, 100, 100, 100, 2, 3, 4, 100 }));
	EXPECT_EQ(DownloadWords(ctx, src, 8), (std::vector<uint32_t>{ 0, 1, 2, 3, 4, 5, 6, 7 }));
}

TEST(Buffer, FillBuffer_Range_SetsOnlyThatRange)
{
	lgx::GraphicsContext ctx("Test context");

	lgx::Buffer buffer = CreateWordBuffer(ctx, std::vector<uint32_t>(8, 1));

	lgx::CommandBuffer cmd_buf(ctx);
	cmd_buf.FillBuffer(buffer, 0xDEADBEEF, 2 * sizeof(uint32_t), 3 * sizeof(uint32_t));
	lgx::SubmitAndWait(ctx, cmd_buf);

	EXPECT_EQ(DownloadWords(ctx, buffer, 8), (std::vector<uint32_t>{ 1, 1, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 1, 1, 1 }));
}

TEST(Buffer, FillBuffer_WholeSize_SetsEverythingFromTheOffset)
{
	lgx::GraphicsContext ctx("Test context");

	lgx::Buffer buffer = CreateWordBuffer(ctx, std::vector<uint32_t>(8, 1));

	lgx::CommandBuffer cmd_buf(ctx);
	cmd_buf.FillBuffer(buffer, 7, 4 * sizeof(uint32_t));
	lgx::SubmitAndWait(ctx, cmd_buf);

	EXPECT_EQ(DownloadWords(ctx, buffer, 8), (std::vector<uint32_t>{ 1, 1, 1, 1, 7, 7, 7, 7 }));
}

TEST(Buffer, BindBuffer_WithAnOffset_ShaderSeesOnlyThatPart)
{
	lgx::GraphicsContext ctx("Test context");

	/* Three slots, where the one in the middle is what the shader gets as its whole buffer */
	const uint32_t slot_words = ctx.MinStorageBufferOffsetAlignment() / sizeof(uint32_t);
	std::vector<uint32_t> words(3 * slot_words);
	for (uint32_t i = 0; i < words.size(); i++)
	{
		words[i] = i + 1;
	}
	lgx::Buffer buffer = CreateWordBuffer(ctx, words);

	const std::string cs_path = std::string(TESTS_SHADER_DIR) + "/DoubleBuffer.cs.spv";
	lgx::ComputePipelineInfo pipeline_info{};
	pipeline_info.cs_name = cs_path.c_str();
	pipeline_info.AddStorageBuffer(0);
	lgx::ComputePipeline pipeline(ctx, pipeline_info);

	constexpr uint32_t part_words = 4;
	lgx::CommandBuffer cmd_buf(ctx);
	cmd_buf.BindPipeline(pipeline);
	cmd_buf.BindBuffer(buffer, 0, slot_words * sizeof(uint32_t), part_words * sizeof(uint32_t));
	cmd_buf.Dispatch(part_words, 1, 1);
	cmd_buf.Barrier(buffer, lgx::ResourceUsage::ComputeWrite, lgx::ResourceUsage::TransferSrc);
	lgx::SubmitAndWait(ctx, cmd_buf);

	std::vector<uint32_t> expected = words;
	for (uint32_t i = slot_words; i < slot_words + part_words; i++)
	{
		expected[i] = words[i] * 2;
	}
	EXPECT_EQ(DownloadWords(ctx, buffer, static_cast<uint32_t>(words.size())), expected);
}

TEST(Buffer, UniformBuffer_AtAnOffset_IsReadByTheShaderAfterAUniformReadBarrier)
{
	lgx::GraphicsContext ctx("Test context");

	const uint32_t alignment = ctx.MinUniformBufferOffsetAlignment();
	lgx::Buffer uniform_buffer(ctx, 2 * alignment, lgx::BufferType::UniformBuffer);
	lgx::Buffer results = CreateWordBuffer(ctx, std::vector<uint32_t>(4, 0));

	const std::string cs_path = std::string(TESTS_SHADER_DIR) + "/UniformCopy.cs.spv";
	lgx::ComputePipelineInfo pipeline_info{};
	pipeline_info.cs_name = cs_path.c_str();
	pipeline_info.AddUniformBuffer(0);
	pipeline_info.AddStorageBuffer(1);
	lgx::ComputePipeline pipeline(ctx, pipeline_info);

	const std::vector<uint32_t> values = { 5, 6, 7, 8 };
	lgx::CommandBuffer cmd_buf(ctx);
	cmd_buf.ScheduleUpload(values.data(), values.size() * sizeof(uint32_t), uniform_buffer, alignment);
	cmd_buf.Barrier(uniform_buffer, lgx::ResourceUsage::TransferDst, lgx::ResourceUsage::UniformRead);
	cmd_buf.BindPipeline(pipeline);
	cmd_buf.BindBuffer(uniform_buffer, 0, alignment, values.size() * sizeof(uint32_t));
	cmd_buf.BindBuffer(results, 1);
	cmd_buf.Dispatch(1, 1, 1);
	cmd_buf.Barrier(results, lgx::ResourceUsage::ComputeWrite, lgx::ResourceUsage::TransferSrc);
	lgx::SubmitAndWait(ctx, cmd_buf);

	EXPECT_EQ(DownloadWords(ctx, results, 4), values);
}

TEST(Buffer, PushConstants_ChangeBetweenDispatches)
{
	lgx::GraphicsContext ctx("Test context");

	lgx::Buffer results = CreateWordBuffer(ctx, std::vector<uint32_t>(4, 0));

	const std::string cs_path = std::string(TESTS_SHADER_DIR) + "/PushConstants.cs.spv";
	lgx::ComputePipelineInfo pipeline_info{};
	pipeline_info.cs_name = cs_path.c_str();
	pipeline_info.push_constant_size = 2 * sizeof(uint32_t);
	pipeline_info.AddStorageBuffer(0);
	lgx::ComputePipeline pipeline(ctx, pipeline_info);

	/* The shader writes value * 2 at index */
	const uint32_t first[2] = { 0, 10 };
	const uint32_t second[2] = { 3, 21 };
	lgx::CommandBuffer cmd_buf(ctx);
	cmd_buf.BindPipeline(pipeline);
	cmd_buf.BindBuffer(results, 0);
	cmd_buf.PushConstants(first, sizeof(first));
	cmd_buf.Dispatch(1, 1, 1);
	cmd_buf.PushConstants(second, sizeof(second));
	cmd_buf.Dispatch(1, 1, 1);
	cmd_buf.Barrier(results, lgx::ResourceUsage::ComputeWrite, lgx::ResourceUsage::TransferSrc);
	lgx::SubmitAndWait(ctx, cmd_buf);

	EXPECT_EQ(DownloadWords(ctx, results, 4), (std::vector<uint32_t>{ 20, 0, 0, 42 }));
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}