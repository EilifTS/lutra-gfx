#include <iostream>
#include <efvk/Window.h>
#include <efvk/GraphicsContext.h>
#include <efvk/FrameManager.h>
#include <efvk/SpriteBatch.h>

int main()
{
	efvk::Window window(800, 600, "EFVK");

	efvk::GraphicsContext graphics_context(window, "EFVK");
	efvk::FrameManager frame_manager(graphics_context, 800, 600);
	efvk::SpriteBatch sb(graphics_context);

	while (window.IsOpen()) {
		window.RetrieveEvents();
		frame_manager.StartFrame(graphics_context);

		sb.Begin();

		const efvk::Sprite s{
			.position = ef::vec2(0.5f, 0.5f),
			.size = ef::vec2(10.0f, 10.0f),
			.color = ef::Color::Blue(),
		};
		sb.Draw(s);
		sb.End(frame_manager);

		frame_manager.EndFrame(graphics_context);
	}

	graphics_context.WaitIdle();

	return 0;
}
