#include <gtest/gtest.h>
#include <lutra-gfx/GraphicsContext.h>
#include <lutra-gfx/Buffer.h>
#include <lutra-gfx/CommandBuffer.h>
#include <vector>
#include <cstdint>

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

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}