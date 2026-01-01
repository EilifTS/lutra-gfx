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

	/* Create camera */
	ef::Camera2D cam(ef::vec2(window.Width(), window.Height()));
	cam.GetPosition() = cam.GetCameraSize() * 0.5f;

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

		sb.Begin(cam);

		const efvk::Sprite s1{
			.position = ef::vec2(5.0f, 5.0f),
			.size = ef::vec2(10.0f, 10.0f),
			.texture = &texture1,
			.color = ef::Color::Red(),
		};
		const efvk::Sprite s2{
			.position = ef::vec2(400.0f, 150.0f),
			.size = ef::vec2(10.0f, 300.0f),
			.color = ef::Color::Red(),
		};
		const efvk::Sprite s3{
			.position = ef::vec2(500.0f, 150.0f),
			.size = ef::vec2(16.0f, 16.0f),
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
