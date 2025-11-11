#include "GraphicsContext.h"
#include <GLFW/glfw3.h>

/* Vulkan HPP boiler plate for setting up a dispatcher */
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;

namespace efvk
{
	GraphicsContext::GraphicsContext(const Window& window, const char* app_name)
	{
		vk::Result result = vk::Result::eSuccess;

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
		vk::ApplicationInfo app_info{
			.pApplicationName = "",
			.applicationVersion = 0,
			.pEngineName = "EFVK",
			.engineVersion = 0,
			.apiVersion = vk::ApiVersion13,
		};

		vk::InstanceCreateInfo instance_create_info{
			.pApplicationInfo = &app_info,
			.enabledExtensionCount = static_cast<u32>(instance_extensions.size()),
			.ppEnabledExtensionNames = instance_extensions.data(),
		};

		std::tie(result, instance) = vk::createInstance(instance_create_info);
		assert(result == vk::Result::eSuccess);

		/* Second initialize step of the dispatcher */
		VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);

		/* Create surface */
		GLFWwindow* glfw_window = static_cast<GLFWwindow*>(window.GetHandle());
		VkSurfaceKHR temp_surface = VK_NULL_HANDLE;
		result = static_cast<vk::Result>(glfwCreateWindowSurface(instance, glfw_window, nullptr, &temp_surface));
		assert(result == vk::Result::eSuccess);
		surface = temp_surface;
		
		/* Get physical device */
		std::vector<vk::PhysicalDevice> physical_devices;
		std::tie(result, physical_devices) = instance.enumeratePhysicalDevices();
		assert(result == vk::Result::eSuccess);

		bool found_physical_device = false;
		for (vk::PhysicalDevice& pd : physical_devices)
		{
			std::vector<vk::QueueFamilyProperties> queue_family_props = pd.getQueueFamilyProperties();

			for (u32 i = 0; i < queue_family_props.size(); i++)
			{
				if (!!(queue_family_props[i].queueFlags & vk::QueueFlagBits::eGraphics))
				{
					u32 has_surface_support = 0;
					std::tie(result, has_surface_support) = pd.getSurfaceSupportKHR(i, surface);
					assert(result == vk::Result::eSuccess);

					if (has_surface_support == 1)
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
		vk::DeviceQueueCreateInfo queue_create_info{
			.queueFamilyIndex = queue_family_index,
			.queueCount = 1,
			.pQueuePriorities = &queue_priority,
		};
		std::vector<vk::ExtensionProperties> supported_device_extensions;
		std::tie(result, supported_device_extensions) = physical_device.enumerateDeviceExtensionProperties();
		assert(result == vk::Result::eSuccess);

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

		vk::DeviceCreateInfo device_info{
			.queueCreateInfoCount = 1,
			.pQueueCreateInfos = &queue_create_info,
			.enabledExtensionCount = static_cast<u32>(required_device_extensions.size()),
			.ppEnabledExtensionNames = required_device_extensions.data(),
		};
		std::tie(result, device) = physical_device.createDevice(device_info);
		assert(result == vk::Result::eSuccess);

		/* Third initialize step of the dispatcher */
		VULKAN_HPP_DEFAULT_DISPATCHER.init(device);

		/* Get the device queue */
		queue = device.getQueue(queue_family_index, 0);
	}
}