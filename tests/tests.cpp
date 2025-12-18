#include <gtest/gtest.h>
#include <efvk/GraphicsContext.h>

TEST(Graphics, CreateContext)
{
	efvk::GraphicsContext ctx("Test context");
	(void)ctx;
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}