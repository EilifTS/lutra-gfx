#pragma once
#ifdef USE_IMGUI

#include <imgui.h>

namespace lgx
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

		void Image(const lgx::Texture& texture, float width, float height);
		void Image(const lgx::Texture& texture, float src_x, float src_y, float src_w, float src_h, float width, float height);
		bool ImageButton(const char* id, const lgx::Texture& texture, float src_x, float src_y, float src_w, float src_h, float width, float height);
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

