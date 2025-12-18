#include <efvk/GraphicsContext.h>

#include "GraphicsContextInternal.h"
#include <GLFW/glfw3.h>
#include <iostream>

/* Vulkan HPP boiler plate for setting up a dispatcher */
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;

#if ENABLE_PORTABILITY
#include <vulkan/vulkan_beta.h>
#endif

#if _DEBUG /* VL */

VKAPI_ATTR vk::Bool32 VKAPI_CALL debug_utils_messenger_callback(
	vk::DebugUtilsMessageSeverityFlagBitsEXT		message_severity,
	vk::DebugUtilsMessageTypeFlagsEXT				message_type,
	const vk::DebugUtilsMessengerCallbackDataEXT* callback_data,
	void* user_data)
{
	if (message_severity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
	{
		std::cout << "VL WARNING " << callback_data->messageIdNumber << " " << callback_data->pMessageIdName << ": " << callback_data->pMessage << std::endl;
	}
	else if (message_severity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
	{
		std::cout << "VL ERROR " << callback_data->messageIdNumber << " " << callback_data->pMessageIdName << ": " << callback_data->pMessage << std::endl;
	}
	return VK_FALSE;
}

static vk::DebugUtilsMessengerCreateInfoEXT create_messenger_info()
{
	return vk::DebugUtilsMessengerCreateInfoEXT{
		.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning,
		.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
		.pfnUserCallback = debug_utils_messenger_callback,
	};
}

#endif

namespace efvk
{
	vk::UniqueInstance create_instance(const char* app_name, bool has_window)
	{
		/* Define layers */
		std::vector<const char*> requested_layers{};
#if _DEBUG /* VL */
		requested_layers.push_back("VK_LAYER_KHRONOS_validation");
#endif
		std::vector<vk::LayerProperties> supported_layers = vk::enumerateInstanceLayerProperties();
		std::vector<const char*> layers{};
		for (const char* requested_layer : requested_layers)
		{
			bool found = false;
			for (const vk::LayerProperties& supported_layer : supported_layers)
			{
				if (strcmp(requested_layer, supported_layer.layerName) == 0)
				{
					found = true;
					break;
				}
			}
			if (found)
			{
				std::cout << "Layer enabled: " << requested_layer << std::endl;
				layers.push_back(requested_layer);
			}
			else
			{
				std::cout << "Warning layer not enabled: " << requested_layer << std::endl;
			}
		}

		/* Define instance extensions */
		std::vector<const char*> instance_extensions{};

#if _DEBUG /* VL */
		instance_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
#if ENABLE_PORTABILITY
		instance_extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif

		/* Get extensions needed by GLFW */
		if (has_window)
		{
			u32 glwf_instance_extension_count = 0;
			const char** glwf_instance_extension = glfwGetRequiredInstanceExtensions(&glwf_instance_extension_count);

			for (u32 i = 0; i < glwf_instance_extension_count; i++)
			{
				instance_extensions.push_back(glwf_instance_extension[i]);
			}
		}

#if _DEBUG /* VL */
		const auto messenger_info = create_messenger_info();
#endif

		/* Create instance */
		const vk::ApplicationInfo app_info{
			.pApplicationName = app_name,
			.applicationVersion = 0,
			.pEngineName = "EFVK",
			.engineVersion = 0,
			.apiVersion = vk::ApiVersion13,
		};

		const vk::InstanceCreateInfo instance_create_info{
#if _DEBUG /* VL */
			.pNext = &messenger_info,
#endif
#if ENABLE_PORTABILITY
			.flags = vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR,
#endif
			.pApplicationInfo = &app_info,
			.enabledLayerCount = static_cast<u32>(layers.size()),
			.ppEnabledLayerNames = layers.data(),
			.enabledExtensionCount = static_cast<u32>(instance_extensions.size()),
			.ppEnabledExtensionNames = instance_extensions.data(),
		};

		return vk::createInstanceUnique(instance_create_info);
	}

	vk::UniqueSurfaceKHR create_surface(vk::Instance instance, const Window& window)
	{
		GLFWwindow* glfw_window = static_cast<GLFWwindow*>(window.GetHandle());
		VkSurfaceKHR temp_surface = VK_NULL_HANDLE;
		VkResult result = glfwCreateWindowSurface(instance, glfw_window, nullptr, &temp_surface);
		assert(result == VK_SUCCESS);
		return vk::UniqueSurfaceKHR(temp_surface, instance);
	}

	static bool physicalDeviceCompatible(vk::PhysicalDevice pd)
	{
#if ENABLE_PORTABILITY
		const auto supported_device_extensions = pd.enumerateDeviceExtensionProperties();
		const bool supports_portability = std::end(supported_device_extensions) != std::find_if(
			std::begin(supported_device_extensions),
			std::end(supported_device_extensions),
			[](vk::ExtensionProperties props)
			{
				return strcmp(props.extensionName, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME) == 1;
			}
		);
		return supports_portability;
#else
		(void)pd;
		return true;
#endif
	}

	static bool queueFamilyCompatible(vk::PhysicalDevice pd, vk::SurfaceKHR surface, const vk::QueueFamilyProperties props, u32 family_index)
	{
		/* The queue family have to support graphics operations */
		if (!(props.queueFlags & vk::QueueFlagBits::eGraphics)) return false;

		/* If we are not using a window, then no more requirements are needed */
		if (surface == VK_NULL_HANDLE) return true;

		/* Otherwise check for surface support */
		auto has_surface_support = pd.getSurfaceSupportKHR(family_index, surface);
		if (!has_surface_support) return false;

		return true;
	}

	std::pair<vk::PhysicalDevice, u32> select_physical_device_and_queue_family(vk::Instance instance, vk::SurfaceKHR surface)
	{
		std::vector<vk::PhysicalDevice> physical_devices = instance.enumeratePhysicalDevices();

		vk::PhysicalDevice physical_device = VK_NULL_HANDLE;
		u32 queue_family_index = 0;
		for (vk::PhysicalDevice& pd : physical_devices)
		{
			if (!physicalDeviceCompatible(pd))
			{
				continue;
			}

			std::vector<vk::QueueFamilyProperties> queue_family_props = pd.getQueueFamilyProperties();

			for (u32 i = 0; i < queue_family_props.size(); i++)
			{
				if (queueFamilyCompatible(pd, surface, queue_family_props[i], i))
				{
					queue_family_index = i;
					physical_device = pd;
					break;
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
		required_device_extensions.push_back(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
		required_device_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

#if ENABLE_PORTABILITY
		required_device_extensions.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
#endif

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
			if (!extension_supported)
			{
				std::cout << ex_name << std::endl;
			}
			assert(extension_supported);
		}

		vk::PhysicalDeviceVulkan13Features features13{
			.dynamicRendering = true,
		};

		vk::PhysicalDeviceFeatures2 features2{
			.pNext = &features13,
		};

		const vk::DeviceCreateInfo device_info{
			.pNext = &features2,
			.queueCreateInfoCount = 1,
			.pQueueCreateInfos = &queue_create_info,
			.enabledExtensionCount = static_cast<u32>(required_device_extensions.size()),
			.ppEnabledExtensionNames = required_device_extensions.data(),
		};
		return physical_device.createDeviceUnique(device_info);
	}

	GraphicsContextInternal::GraphicsContextInternal(const char* app_name)
	{
		init(app_name, nullptr);
	}

	GraphicsContextInternal::GraphicsContextInternal(const char* app_name, const Window& window)
	{
		init(app_name, &window);
	}

	GraphicsContextInternal::~GraphicsContextInternal()
	{
		if (device.get() != nullptr)
		{
			device->waitIdle();
		}

		if (vma_allocator != VK_NULL_HANDLE)
		{
			vmaDestroyAllocator(vma_allocator);
		}
	}

	void GraphicsContextInternal::WaitIdle()
	{
		device->waitIdle();
	}

	void GraphicsContextInternal::init(const char* app_name, const Window* window)
	{
		/* First initialize step of the dispatcher */
		VULKAN_HPP_DEFAULT_DISPATCHER.init();

		const bool has_window = window != nullptr;

		/* Create instance */
		instance = create_instance(app_name, has_window);

		/* Second initialize step of the dispatcher */
		VULKAN_HPP_DEFAULT_DISPATCHER.init(*instance);

#if _DEBUG /* VL */
		messenger = instance->createDebugUtilsMessengerEXTUnique(create_messenger_info());
#endif

		/* Create surface */
		if (has_window)
		{
			surface = create_surface(*instance, *window);
		}

		/* Get physical device */
		std::tie(physical_device, queue_family_index) = select_physical_device_and_queue_family(instance.get(), *surface);

		/* Create the logical device */
		device = create_device(physical_device, queue_family_index);

		/* Third initialize step of the dispatcher */
		VULKAN_HPP_DEFAULT_DISPATCHER.init(*device);

		/* Get the device queue */
		queue = device->getQueue(queue_family_index, 0);

		/* Create the Vulkan Memory Allocator */
		VmaVulkanFunctions vma_vk_functions{};
		vma_vk_functions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
		vma_vk_functions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

		VmaAllocatorCreateInfo vma_info{};
		vma_info.instance = *instance;
		vma_info.physicalDevice = physical_device;
		vma_info.device = *device;
		vma_info.vulkanApiVersion = vk::ApiVersion13;
		vma_info.pVulkanFunctions = &vma_vk_functions;
		VkResult result = vmaCreateAllocator(&vma_info, &vma_allocator);
		assert(result == VK_SUCCESS);

		/* Create the command pool */
		const vk::CommandPoolCreateInfo pool_info{
			.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
			.queueFamilyIndex = queue_family_index,
		};
		cmd_pool = device->createCommandPoolUnique(pool_info);
	}
}