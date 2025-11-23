#include <iostream>
#include <efvk/Window.h>
#include <efvk/GraphicsContext.h>
#include <efvk/FrameManager.h>
#include <efvk/SpriteBatch.h>
#include <efvk/Image.h>
#include <efvk/Texture.h>

int main()
{
	efvk::Window window(800, 600, "EFVK");

	efvk::GraphicsContext graphics_context(window, "EFVK");
	efvk::FrameManager frame_manager(graphics_context, 800, 600);
	efvk::SpriteBatch sb(graphics_context);

	/* Temp image load test */
	efvk::Image test_img{};
	test_img.Load("textures/test.png");
	efvk::Texture texture(graphics_context, test_img);

	while (window.IsOpen()) 
	{
		window.RetrieveEvents();
		frame_manager.StartFrame(graphics_context);

		sb.Begin();

		const efvk::Sprite s1{
			.position = ef::vec2(5.0f, 5.0f),
			.size = ef::vec2(10.0f, 10.0f),
			.color = ef::Color::Blue(),
		};
		const efvk::Sprite s2{
			.position = ef::vec2(400.0f, 150.0f),
			.size = ef::vec2(10.0f, 300.0f),
			.color = ef::Color::Blue(),
		};
		sb.Draw(s1);
		sb.Draw(s2);
		sb.End(frame_manager);

		frame_manager.EndFrame(graphics_context);
	}

	graphics_context.WaitIdle();

	return 0;
}
