#include "GraphicsContext.h"

namespace efvk
{
	GraphicsContext::GraphicsContext(const Window& window)
	{
		vk::ApplicationInfo app_info{};
		app_info.pApplicationName = "EFVK";
		app_info.applicationVersion = 0;
		app_info.pEngineName = "EFVK";
		app_info.engineVersion = 0;
		app_info.apiVersion = vk::ApiVersion13;

		vk::InstanceCreateInfo instance_create_info{};
		instance_create_info.pApplicationInfo = &app_info;

		instance = vk::createInstance(instance_create_info);
	}
}