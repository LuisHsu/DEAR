#include <Backend.hpp>

void Backend::initVK(BackendBase* &backend, BackendBase::LibType &lib, BackendBase::WMType &wm){
/*** Instance ***/
	// App info
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "DEAR";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 59);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;
	// Create info
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledLayerCount = 0;
	// Extensions
	createInfo.enabledExtensionCount = 2;
	const char *extensionNames[createInfo.enabledExtensionCount];
	extensionNames[0] = "VK_KHR_surface";
	createInfo.ppEnabledExtensionNames = extensionNames;
	// Check enabled extensions
	bool displaySupport = false;
	bool xcbSupport = false;
	bool waylandSupport = false;
	bool surfaceSupport = false;
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	VkExtensionProperties extensions[extensionCount];
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions);
	for (const VkExtensionProperties& extension : extensions) {
		if(!strcmp(extension.extensionName, "VK_KHR_display")){
			displaySupport = true;
		}
		if(!strcmp(extension.extensionName, "VK_KHR_xcb_surface")){
			xcbSupport = true;
		}
		if(!strcmp(extension.extensionName, "VK_KHR_wayland_surface")){
			waylandSupport = true;
		}
		if(!strcmp(extension.extensionName, "VK_KHR_surface")){
			surfaceSupport = true;
		}
	}
	// Check surface support
	if(!surfaceSupport){
		throw "[Vulkan Extension] VK_KHR_surface is unsupported.";
	}else if(wm == BackendBase::WMType::WAYLAND_WM && !waylandSupport){
		throw "[Vulkan Extension] VK_KHR_wayland_surface is unsupported.";
	}else if(wm == BackendBase::WMType::XCB_WM && !xcbSupport){
		throw "[Vulkan Extension] VK_KHR_xcb_surface is unsupported.";
	}else if(wm == BackendBase::WMType::DISPLAY_WM && !displaySupport){
		throw "[Vulkan Extension] VK_KHR_display is unsupported.";
	}
	// Create backend
#ifdef WAYLAND_SUPPORT
	if(waylandSupport){
		extensionNames[1] = "VK_KHR_wayland_surface";
		backend = new BackendWayland(BackendBase::LibType::VULKAN_LIB);
		switch (vkCreateInstance(&createInfo, nullptr, &backend->vkInstance)){
			case VK_ERROR_OUT_OF_HOST_MEMORY:
				throw "VK_ERROR_OUT_OF_HOST_MEMORY";
			break;
			case VK_ERROR_OUT_OF_DEVICE_MEMORY:
				throw "VK_ERROR_OUT_OF_DEVICE_MEMORY";
			break;
			case VK_ERROR_INITIALIZATION_FAILED:
				throw "VK_ERROR_INITIALIZATION_FAILED";
			break;
			case VK_ERROR_LAYER_NOT_PRESENT:
				throw "VK_ERROR_LAYER_NOT_PRESENT";
			break;
			case VK_ERROR_EXTENSION_NOT_PRESENT:
				throw "VK_ERROR_EXTENSION_NOT_PRESENT";
			break;
			case VK_ERROR_INCOMPATIBLE_DRIVER:
				throw "VK_ERROR_INCOMPATIBLE_DRIVER";
			break;
			default:
			break;
		}
		lib = BackendBase::LibType::VULKAN_LIB;
		wm = BackendBase::WMType::WAYLAND_WM;
	}else
#endif
#ifdef XCB_SUPPORT
	if(xcbSupport){
		extensionNames[1] = "VK_KHR_xcb_surface";
		backend = new BackendXcb(BackendBase::LibType::VULKAN_LIB);
		switch (vkCreateInstance(&createInfo, nullptr, &backend->vkInstance)){
			case VK_ERROR_OUT_OF_HOST_MEMORY:
				throw "VK_ERROR_OUT_OF_HOST_MEMORY";
			break;
			case VK_ERROR_OUT_OF_DEVICE_MEMORY:
				throw "VK_ERROR_OUT_OF_DEVICE_MEMORY";
			break;
			case VK_ERROR_INITIALIZATION_FAILED:
				throw "VK_ERROR_INITIALIZATION_FAILED";
			break;
			case VK_ERROR_LAYER_NOT_PRESENT:
				throw "VK_ERROR_LAYER_NOT_PRESENT";
			break;
			case VK_ERROR_EXTENSION_NOT_PRESENT:
				throw "VK_ERROR_EXTENSION_NOT_PRESENT";
			break;
			case VK_ERROR_INCOMPATIBLE_DRIVER:
				throw "VK_ERROR_INCOMPATIBLE_DRIVER";
			break;
			default:
			break;
		}
		lib = BackendBase::LibType::VULKAN_LIB;
		wm = BackendBase::WMType::XCB_WM;
	}else
#endif
	if(displaySupport){
		extensionNames[1] = "VK_KHR_display";
		backend = new BackendDisplay(BackendBase::LibType::VULKAN_LIB);
		switch (vkCreateInstance(&createInfo, nullptr, &backend->vkInstance)){
			case VK_ERROR_OUT_OF_HOST_MEMORY:
				throw "VK_ERROR_OUT_OF_HOST_MEMORY";
			break;
			case VK_ERROR_OUT_OF_DEVICE_MEMORY:
				throw "VK_ERROR_OUT_OF_DEVICE_MEMORY";
			break;
			case VK_ERROR_INITIALIZATION_FAILED:
				throw "VK_ERROR_INITIALIZATION_FAILED";
			break;
			case VK_ERROR_LAYER_NOT_PRESENT:
				throw "VK_ERROR_LAYER_NOT_PRESENT";
			break;
			case VK_ERROR_EXTENSION_NOT_PRESENT:
				throw "VK_ERROR_EXTENSION_NOT_PRESENT";
			break;
			case VK_ERROR_INCOMPATIBLE_DRIVER:
				throw "VK_ERROR_INCOMPATIBLE_DRIVER";
			break;
			default:
			break;
		}
		lib = BackendBase::LibType::VULKAN_LIB;
		wm = BackendBase::WMType::DISPLAY_WM;
	}else{
		throw "[Create Backend] No any protocol support and display is unsupported.";
	}
	backend->createSurface();
/*** Physical devices ***/
	// Find a device
	backend->vkPhyDevice = pickPhyDevice(backend->vkInstance, wm);
	// Get queue families
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(backend->vkPhyDevice, &queueFamilyCount, nullptr);
	VkQueueFamilyProperties queueFamilies[queueFamilyCount];
	vkGetPhysicalDeviceQueueFamilyProperties(backend->vkPhyDevice, &queueFamilyCount, queueFamilies);
	for (uint32_t i = 0; i < queueFamilyCount; ++i){
		// Graphics
		if (queueFamilies[i].queueCount > 0 && queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			backend->graphicsFamily = i;
		}
	}
}

VkPhysicalDevice Backend::pickPhyDevice(VkInstance instance, BackendBase::WMType &wm){
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	VkPhysicalDevice devices[deviceCount];
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices);
	uint32_t bestScore = 0;
	int bestIndex = -1;
	for(uint32_t i = 0; i < deviceCount; ++i){
		uint32_t score = 0;
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(devices[i], &deviceProperties);
		// DISPLAY_WM must have a display
		//if(wm == BackendBase::WMType::DISPLAY_WM){
			uint32_t displayCount = 0;
			vkGetPhysicalDeviceDisplayPropertiesKHR(devices[i], &displayCount, nullptr);
			if(displayCount == 0){
				continue;
			}
		//}
		// Device type
		switch(deviceProperties.deviceType){
			case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
				score += 40000;
			break;
			case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
				score += 30000;
			break;
			case VK_PHYSICAL_DEVICE_TYPE_CPU:
				score += 20000;
			break;
			case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
				score += 10000;
			break;
			default:
			break;
		}
		// Limits
		score += deviceProperties.limits.maxImageDimension2D;
		score += deviceProperties.limits.maxImageDimension3D;
		// Compare
		if(score > bestScore){
			bestScore = score;
			bestIndex = i;
		}
	}
	// Throw error if no device
	if(bestIndex == -1){
		throw "No suitable device.";
	}else{
		return devices[bestIndex];
	}
}