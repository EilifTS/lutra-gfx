#include "GraphicsContext.h"
#include <GLFW/glfw3.h>

/* Vulkan HPP boiler plate for setting up a dispatcher */
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;

namespace efvk
{
	GraphicsContext::GraphicsContext(const Window& window)
	{
		/* First initialize step of the dispatcher */
		VULKAN_HPP_DEFAULT_DISPATCHER.init();

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

		/* Second initialize step of the dispatcher */
		VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);

		/* Create surface */
		GLFWwindow* glfw_window = static_cast<GLFWwindow*>(window.GetHandle());
		VkSurfaceKHR temp_surface = VK_NULL_HANDLE;
		VkResult result = glfwCreateWindowSurface(instance, glfw_window, nullptr, &temp_surface);
		assert(result == VK_SUCCESS);
		surface = temp_surface;
		
		/* Get physical device */
		std::vector<vk::PhysicalDevice> physical_devices = instance.enumeratePhysicalDevices();
		bool found_physical_device = false;
		for (vk::PhysicalDevice& pd : physical_devices)
		{
			std::vector<vk::QueueFamilyProperties> queue_family_props = pd.getQueueFamilyProperties();

			for (u32 i = 0; i < queue_family_props.size(); i++)
			{
				if (!!(queue_family_props[i].queueFlags & vk::QueueFlagBits::eGraphics))
				{
					if (pd.getSurfaceSupportKHR(i, surface))
					{
						queue_family_index = i;
						found_physical_device = true;
						physical_device = pd;
					}
				}
			}
		}
		assert(found_physical_device);

		/* Create the logical device */
		constexpr float queue_priority = 1.0f;
		vk::DeviceQueueCreateInfo queue_create_info{};
		queue_create_info.queueFamilyIndex = queue_family_index;
		queue_create_info.pQueuePriorities = &queue_priority;
		queue_create_info.queueCount = 1;

		std::vector<vk::ExtensionProperties> supported_device_extensions = physical_device.enumerateDeviceExtensionProperties();

		std::vector<const char*> required_device_extensions{};
		required_device_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

		for (const char* ex_name : required_device_extensions)
		{
			bool extension_supported = false;
			for (const vk::ExtensionProperties& prop : supported_device_extensions)
			{
				if (strcmp(ex_name, prop.extensionName) == 0)
				{
					extension_supported = true;
					break;
				}
			}
			assert(extension_supported);
		}

		vk::DeviceCreateInfo device_info{};
		device_info.enabledExtensionCount = required_device_extensions.size();
		device_info.ppEnabledExtensionNames = required_device_extensions.data();
		device_info.queueCreateInfoCount = 1;
		device_info.pQueueCreateInfos = &queue_create_info;
		device = physical_device.createDevice(device_info);

		/* Third initialize step of the dispatcher */
		VULKAN_HPP_DEFAULT_DISPATCHER.init(device);

		/* Get the device queue */
		queue = device.getQueue(queue_family_index, 0);
	}
}