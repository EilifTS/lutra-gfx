#include <iostream>
#include "Window.h"
#include "GraphicsContext.h"
#include "FrameManager.h"

int main()
{
	efvk::Window window(800, 600, "EFVK");

	efvk::GraphicsContext graphics_context(window);
	efvk::FrameManager frame_manager(graphics_context, 800, 600);

	while (window.IsOpen()) {
		window.RetrieveEvents();
		frame_manager.StartFrame(graphics_context);
		frame_manager.EndFrame(graphics_context);
	}

	return 0;
}
