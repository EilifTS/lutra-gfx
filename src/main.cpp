#include <iostream>
#include "Window.h"

int main()
{
	ef::Window window(800, 600, "EFVK");

	while (window.IsOpen()) {
		window.RetrieveEvents();
	}

	return 0;
}
