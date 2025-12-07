#pragma once
#include <efvk/math/Rectangle.h>

namespace efvk
{
	class Window;
	class GraphicsContext;
	class FrameManager;

	namespace ImGuiWrapper
	{
		void Initialize(Window& window, GraphicsContext& ctx);
		void StartFrame();
		void EndFrame(FrameManager& frame_manager);
		void ShutDown();

		//void Image(const ef::Texture2D& texture, const ef::vec2& size);
		//void Image(const ef::Texture2D& texture, const ef::Rectanglef& src_rect, const ef::vec2& size);
		//bool ImageButton(const char* id, const ef::Texture2D& texture, const ef::Rectanglef& src_rect, const ef::vec2& size);
	}
}

