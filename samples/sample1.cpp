#include <iostream>
#include <efvk/Window.h>
#include <efvk/GraphicsContext.h>
#include <efvk/FrameManager.h>
#include <efvk/Image.h>
#include <efvk/Texture.h>
#include <efvk/ImGuiWrapper.h>

#include "SpriteBatch.h"

int main()
{
	efvk::Window window(800, 600, "EFVK");

	efvk::GraphicsContext graphics_context("EFVK", window);
	efvk::FrameManager frame_manager(graphics_context, 800, 600);

#ifdef USE_IMGUI
	efvk::ImGuiWrapperScope imgui_scope(window, graphics_context);
#endif

	efvk::SpriteBatch sb(graphics_context);

	/* Temp image load test */
	efvk::Image test_img1{};
	test_img1.Load("textures/test.png");
	efvk::Texture texture1(graphics_context, test_img1);

	efvk::Image test_img2{};
	test_img2.Load("textures/test2.png");
	efvk::Texture texture2(graphics_context, test_img2);

	while (window.IsOpen())
	{
		window.RetrieveEvents();
		frame_manager.StartFrame(graphics_context);

#ifdef USE_IMGUI
		efvk::ImGuiWrapper::StartFrame();
		ImGui::Begin("Hello ImGui!");
		efvk::ImGuiWrapper::Image(texture2, 128.0f, 128.0f);
		ImGui::End();
#endif

		sb.Begin();

		const efvk::Sprite s1{
			.x = 5.0f, .y = 5.0f,
			.w = 10.0f, .h = 10.0f,
			.color = 0xFF0000FF,
			.texture = &texture1,
		};
		const efvk::Sprite s2{
			.x = 400.0f, .y = 150.0f,
			.w = 10.0f, .h = 300.0f,
			.color = 0xFF0000FF,
		};
		const efvk::Sprite s3{
			.x = 500.0f, .y = 150.0f,
			.w = 16.0f, .h = 16.0f,
			.texture = &texture2,
		};
		sb.Draw(s1);
		sb.Draw(s2);
		sb.Draw(s3);
		sb.End(frame_manager);

#ifdef USE_IMGUI
		efvk::ImGuiWrapper::EndFrame(frame_manager);
#endif

		frame_manager.EndFrame(graphics_context);
	}

	graphics_context.WaitIdle();

	return 0;
}
