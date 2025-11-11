#include "GraphicsContext.h"
#include <GLFW/glfw3.h>

/* Vulkan HPP boiler plate for setting up a dispatcher */
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;

namespace efvk
{
	vk::UniqueInstance create_instance(const char* app_name)
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
		vk::ApplicationInfo app_info{
			.pApplicationName = app_name,
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

		return vk::createInstanceUnique(instance_create_info);
	}

	vk::SurfaceKHR create_surface(vk::Instance instance, const Window& window)
	{
		GLFWwindow* glfw_window = static_cast<GLFWwindow*>(window.GetHandle());
		VkSurfaceKHR temp_surface = VK_NULL_HANDLE;
		VkResult result = glfwCreateWindowSurface(instance, glfw_window, nullptr, &temp_surface);
		assert(result == VK_SUCCESS);
		return temp_surface;
	}

	std::pair<vk::PhysicalDevice, u32> select_physical_device_and_queue_family(vk::Instance instance, vk::SurfaceKHR surface)
	{
		std::vector<vk::PhysicalDevice> physical_devices = instance.enumeratePhysicalDevices();

		vk::PhysicalDevice physical_device = VK_NULL_HANDLE;
		u32 queue_family_index = 0;
		for (vk::PhysicalDevice& pd : physical_devices)
		{
			std::vector<vk::QueueFamilyProperties> queue_family_props = pd.getQueueFamilyProperties();

			for (u32 i = 0; i < queue_family_props.size(); i++)
			{
				if (!!(queue_family_props[i].queueFlags & vk::QueueFlagBits::eGraphics))
				{
					auto has_surface_support = pd.getSurfaceSupportKHR(i, surface);

					if (has_surface_support)
					{
						queue_family_index = i;
						physical_device = pd;
					}
				}
			}
		}
		assert(physical_device != VK_NULL_HANDLE);

		return { physical_device, queue_family_index };
	}

	vk::UniqueDevice create_device(vk::PhysicalDevice physical_device, u32 queue_family_index)
	{
		constexpr float queue_priority = 1.0f;
		vk::DeviceQueueCreateInfo queue_create_info{
			.queueFamilyIndex = queue_family_index,
			.queueCount = 1,
			.pQueuePriorities = &queue_priority,
		};
		auto supported_device_extensions = physical_device.enumerateDeviceExtensionProperties();

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
		return physical_device.createDeviceUnique(device_info);
	}

	GraphicsContext::GraphicsContext(const Window& window, const char* app_name)
	{
		/* First initialize step of the dispatcher */
		VULKAN_HPP_DEFAULT_DISPATCHER.init();

		/* Create instance */
		instance = create_instance(app_name);

		/* Second initialize step of the dispatcher */
		VULKAN_HPP_DEFAULT_DISPATCHER.init(*instance);

		/* Create surface */
		surface = create_surface(*instance, window);
		
		/* Get physical device */
		std::tie(physical_device, queue_family_index) = select_physical_device_and_queue_family(instance.get(), surface);

		/* Create the logical device */
		device = create_device(physical_device, queue_family_index);

		/* Third initialize step of the dispatcher */
		VULKAN_HPP_DEFAULT_DISPATCHER.init(*device);

		/* Get the device queue */
		queue = device->getQueue(queue_family_index, 0);
	}
}