#include <iostream>
#include "Window.h"
#include "GraphicsContext.h"

int main()
{
	efvk::Window window(800, 600, "EFVK");

	efvk::GraphicsContext graphics_context(window);

	while (window.IsOpen()) {
		window.RetrieveEvents();
	}

	return 0;
}
