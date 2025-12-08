#pragma once
#ifdef USE_IMGUI

#include <imgui.h>
#include <efvk/math/Rectangle.h>

namespace efvk
{
	class Window;
	class GraphicsContext;
	class FrameManager;
	class Texture;

	namespace ImGuiWrapper
	{
		void Initialize(Window& window, GraphicsContext& ctx);
		void StartFrame();
		void EndFrame(FrameManager& frame_manager);
		void ShutDown();

		void Image(const efvk::Texture& texture, const ef::vec2& size);
		void Image(const efvk::Texture& texture, const ef::Rectanglef& src_rect, const ef::vec2& size);
		bool ImageButton(const char* id, const efvk::Texture& texture, const ef::Rectanglef& src_rect, const ef::vec2& size);
	}

	class ImGuiWrapperScope
	{
	public:
		ImGuiWrapperScope(Window& window, GraphicsContext& ctx)
		{
			ImGuiWrapper::Initialize(window, ctx);
		}

		~ImGuiWrapperScope()
		{
			ImGuiWrapper::ShutDown();
		}
	};
}

#endif

