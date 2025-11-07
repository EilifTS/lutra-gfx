#include "GraphicsContext.h"
#include <GLFW/glfw3.h>

namespace efvk
{
	GraphicsContext::GraphicsContext(const Window& window)
	{


		std::vector<const char*> instance_extensions{};

		/* Get extensions needed by GLFW */
		{
			u32 glwf_instance_extension_count = 0;
			const char** glwf_instance_extension = glfwGetRequiredInstanceExtensions(&glwf_instance_extension_count);

			for (u32 i = 0; i < glwf_instance_extension_count; i++)
			{
				instance_extensions.push_back(glwf_instance_extension[i]);
			}
		}

		/* Create instance */
		vk::ApplicationInfo app_info{};
		app_info.pApplicationName = "EFVK";
		app_info.applicationVersion = 0;
		app_info.pEngineName = "EFVK";
		app_info.engineVersion = 0;
		app_info.apiVersion = vk::ApiVersion13;

		vk::InstanceCreateInfo instance_create_info{};
		instance_create_info.pApplicationInfo = &app_info;
		instance_create_info.enabledExtensionCount = instance_extensions.size();
		instance_create_info.ppEnabledExtensionNames = instance_extensions.data();

		instance = vk::createInstance(instance_create_info);

		/* Create surface */
		GLFWwindow* glfw_window = static_cast<GLFWwindow*>(window.GetHandle());
		VkSurfaceKHR temp_surface = VK_NULL_HANDLE;
		VkResult result = glfwCreateWindowSurface(instance, glfw_window, nullptr, &temp_surface);
		assert(result == VK_SUCCESS);
		surface = temp_surface;
		
		/* Get physical device */
		std::vector<vk::PhysicalDevice> physical_devices = instance.enumeratePhysicalDevices();
		bool found_physical_device = false;
		int selected_queue_family_index = -1;
		for (vk::PhysicalDevice& pd : physical_devices)
		{
			std::vector<vk::QueueFamilyProperties> queue_family_props = pd.getQueueFamilyProperties();

			for (u32 i = 0; i < queue_family_props.size(); i++)
			{
				if (!!(queue_family_props[i].queueFlags & vk::QueueFlagBits::eGraphics))
				{
					if (pd.getSurfaceSupportKHR(i, surface))
					{
						selected_queue_family_index = i;
						found_physical_device = true;
					}
				}
			}
		}
		assert(found_physical_device);
	}
}