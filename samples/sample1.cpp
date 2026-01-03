#include <iostream>
#include <lutra-gfx/Window.h>
#include <lutra-gfx/GraphicsContext.h>
#include <lutra-gfx/FrameManager.h>
#include <lutra-gfx/Image.h>
#include <lutra-gfx/Texture.h>
#include <lutra-gfx/ImGuiWrapper.h>

#include "SpriteBatch.h"

int main()
{
	lgx::Window window(800, 600, "LutraGFX");

	lgx::GraphicsContext graphics_context("LutraGFX", window);
	lgx::FrameManager frame_manager(graphics_context, 800, 600);

#ifdef USE_IMGUI
	lgx::ImGuiWrapperScope imgui_scope(window, graphics_context);
#endif

	lgx::SpriteBatch sb(graphics_context);

	/* Temp image load test */
	lgx::Image test_img1{};
	test_img1.Load("textures/test.png");
	lgx::Texture texture1(graphics_context, test_img1);

	lgx::Image test_img2{};
	test_img2.Load("textures/test2.png");
	lgx::Texture texture2(graphics_context, test_img2);

	while (window.IsOpen())
	{
		window.RetrieveEvents();
		frame_manager.StartFrame(graphics_context);

#ifdef USE_IMGUI
		lgx::ImGuiWrapper::StartFrame();
		ImGui::Begin("Hello ImGui!");
		lgx::ImGuiWrapper::Image(texture2, 128.0f, 128.0f);
		ImGui::End();
#endif

		sb.Begin();

		const lgx::Sprite s1{
			.x = 5.0f, .y = 5.0f,
			.w = 10.0f, .h = 10.0f,
			.color = 0xFF0000FF,
			.texture = &texture1,
		};
		const lgx::Sprite s2{
			.x = 400.0f, .y = 150.0f,
			.w = 10.0f, .h = 300.0f,
			.color = 0xFF0000FF,
		};
		const lgx::Sprite s3{
			.x = 500.0f, .y = 150.0f,
			.w = 16.0f, .h = 16.0f,
			.texture = &texture2,
		};
		sb.Draw(s1);
		sb.Draw(s2);
		sb.Draw(s3);
		sb.End(frame_manager);

#ifdef USE_IMGUI
		lgx::ImGuiWrapper::EndFrame(frame_manager);
#endif

		frame_manager.EndFrame(graphics_context);
	}

	graphics_context.WaitIdle();

	return 0;
}
