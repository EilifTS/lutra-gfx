#include <iostream>

#include <GLFW/glfw3.h>

int main()
{
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return 1;
	}

	if (!glfwVulkanSupported())
	{
		std::cerr << "GLFW with Vulkan is not supported on this system" << std::endl;
		glfwTerminate();
		return 1;
	}

	/* No OpenGL context */
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window = glfwCreateWindow(800, 600, "EFVK", nullptr, nullptr);
	if (!window)
	{
		std::cerr << "Failed to create window" << std::endl;
		glfwTerminate();
		return 1;
	}

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
