#include <Backend.hpp>

void Backend::initVK(){
/*** Instance ***/
	// App info
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "DEAR Desktop";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;
	// Create info
	VkInstanceCreateInfo instanceCreateInfo = {};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pApplicationInfo = &appInfo;
	instanceCreateInfo.enabledLayerCount = 0;
	// Extensions
	instanceCreateInfo.enabledExtensionCount = 2;
	const char *extensionNames[instanceCreateInfo.enabledExtensionCount];
	extensionNames[0] = "VK_KHR_surface";
	extensionNames[1] = "VK_KHR_display";
	instanceCreateInfo.ppEnabledExtensionNames = extensionNames;
	// Create instance
	switch (vkCreateInstance(&instanceCreateInfo, nullptr, &vkInstance)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan instance] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan instance] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		case VK_ERROR_INITIALIZATION_FAILED:
			throw "[Vulkan instance] VK_ERROR_INITIALIZATION_FAILED";
		break;
		case VK_ERROR_LAYER_NOT_PRESENT:
			throw "[Vulkan instance] VK_ERROR_LAYER_NOT_PRESENT";
		break;
		case VK_ERROR_EXTENSION_NOT_PRESENT:
			throw "[Vulkan instance] VK_ERROR_EXTENSION_NOT_PRESENT";
		break;
		case VK_ERROR_INCOMPATIBLE_DRIVER:
			throw "[Vulkan instance] VK_ERROR_INCOMPATIBLE_DRIVER";
		break;
		default:
			lib = VULKAN;
		break;
	}
/*** Physical device ***/
	// Pick device
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);
	VkPhysicalDevice devices[deviceCount];
	vkEnumeratePhysicalDevices(vkInstance, &deviceCount, devices);
	uint32_t bestScore = 0;
	int bestDevice = -1;
	for(uint32_t i = 0; i < deviceCount; ++i){
		uint32_t score = 0;
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(devices[i], &deviceProperties);
		// Get display
		uint32_t displayCount = 0;
		vkGetPhysicalDeviceDisplayPropertiesKHR(devices[i], &displayCount, nullptr);
		if(displayCount == 0){
			continue;
		}
		VkDisplayPropertiesKHR displayProperties[displayCount];
		vkGetPhysicalDeviceDisplayPropertiesKHR(devices[i], &displayCount, displayProperties);
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
		// Compare
		if(score > bestScore){
			bestScore = score;
			bestDevice = i;
			vkPhyDevice = devices[i];
			vkDisplayProperties = displayProperties[0];
			vkDisplay = displayProperties[0].display;
		}
	}
	// Throw error if no device
	if(bestDevice == -1){
		throw "[Vulkan physical device] No suitable device.";
	}
/*** Surface ***/
	// Pick display mode
	uint32_t displayModePropertyCount = 0;
	vkGetDisplayModePropertiesKHR(vkPhyDevice, vkDisplay, &displayModePropertyCount, nullptr);
	VkDisplayModePropertiesKHR displayModeProperties[displayModePropertyCount];
	vkGetDisplayModePropertiesKHR(vkPhyDevice, vkDisplay, &displayModePropertyCount, displayModeProperties);
	for(uint32_t i = 0; i < displayModePropertyCount; ++i){
		VkExtent2D visibleRegion = displayModeProperties[i].parameters.visibleRegion;
		if(vkDisplayProperties.physicalResolution.width == visibleRegion.width && vkDisplayProperties.physicalResolution.height == visibleRegion.height){
			vkDisplayMode = displayModeProperties[i].displayMode;
			break;
		}
	}
	// Display plane
	uint32_t displayPlanePropertyCount = 0;
	vkGetPhysicalDeviceDisplayPlanePropertiesKHR(vkPhyDevice, &displayPlanePropertyCount, nullptr);
	VkDisplayPlanePropertiesKHR displayPlaneProperties[displayPlanePropertyCount];
	vkGetPhysicalDeviceDisplayPlanePropertiesKHR(vkPhyDevice, &displayPlanePropertyCount, displayPlaneProperties);
	uint32_t planeIndex = 0;
	uint32_t planeStackIndex = 0;
	for(; planeIndex < displayPlanePropertyCount; ++planeIndex){
		if(displayPlaneProperties[planeIndex].currentDisplay == vkDisplay){
			planeStackIndex = displayPlaneProperties[planeIndex].currentStackIndex;
			break;
		}
	}
	// Create surface
	VkDisplaySurfaceCreateInfoKHR surfaceCreateInfo = {};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_DISPLAY_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.displayMode = vkDisplayMode;
	surfaceCreateInfo.planeIndex = planeIndex;
	surfaceCreateInfo.planeStackIndex = planeStackIndex;
	surfaceCreateInfo.transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	surfaceCreateInfo.alphaMode = VK_DISPLAY_PLANE_ALPHA_OPAQUE_BIT_KHR;
	surfaceCreateInfo.imageExtent = vkDisplayProperties.physicalResolution;
	switch(vkCreateDisplayPlaneSurfaceKHR(vkInstance, &surfaceCreateInfo, nullptr, &vkSurface)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan surface] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan surface] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		default:
		break;
	}
/*** Queue family ***/
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(vkPhyDevice, &queueFamilyCount, nullptr);
	VkQueueFamilyProperties queueFamilies[queueFamilyCount];
	vkGetPhysicalDeviceQueueFamilyProperties(vkPhyDevice, &queueFamilyCount, queueFamilies);
	int32_t graphicsFamily = -1;
	int32_t presentFamily = -1;
	for(uint32_t i = 0; i < queueFamilyCount; ++i){
		// Graphics family
		if(queueFamilies[i].queueCount > 0 && queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			graphicsFamily = i;
		}
		// Present family
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(vkPhyDevice, i, vkSurface, &presentSupport);
		if(queueFamilies[i].queueCount > 0 && presentSupport) {
			presentFamily = i;
		}
	}
	if(graphicsFamily < 0 || presentFamily < 0){
		throw "[Vulkan queue family] No suitable queue families.";
	}
/*** Logical device ***/
	// Graphics queue info
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	for(int i = 0; i < 2; ++i){
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = graphicsFamily;
		queueCreateInfo.queueCount = 1;
		float queuePriority = 1.0f;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}
	// Create logical device
	VkPhysicalDeviceFeatures deviceFeatures = {};
	VkDeviceCreateInfo logicalDeviceCreateInfo = {};
	logicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	logicalDeviceCreateInfo.queueCreateInfoCount = 2;
	logicalDeviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	logicalDeviceCreateInfo.pEnabledFeatures = &deviceFeatures;
	logicalDeviceCreateInfo.enabledExtensionCount = 2;
	const char *devExtensionNames[logicalDeviceCreateInfo.enabledExtensionCount];
	devExtensionNames[0] = "VK_KHR_swapchain";
	devExtensionNames[1] = "VK_KHR_display_swapchain";
	logicalDeviceCreateInfo.ppEnabledExtensionNames = devExtensionNames;
	switch(vkCreateDevice(vkPhyDevice, &logicalDeviceCreateInfo, nullptr, &vkDevice)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan device] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan device] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		case VK_ERROR_INITIALIZATION_FAILED:
			throw "[Vulkan device] VK_ERROR_INITIALIZATION_FAILED";
		break;
		case VK_ERROR_EXTENSION_NOT_PRESENT:
			throw "[Vulkan device] VK_ERROR_EXTENSION_NOT_PRESENT";
		break;
		case VK_ERROR_FEATURE_NOT_PRESENT:
			throw "[Vulkan device] VK_ERROR_FEATURE_NOT_PRESENT";
		break;
		case VK_ERROR_TOO_MANY_OBJECTS:
			throw "[Vulkan device] VK_ERROR_TOO_MANY_OBJECTS";
		break;
		case VK_ERROR_DEVICE_LOST:
			throw "[Vulkan device] VK_ERROR_DEVICE_LOST";
		break;
		default:
		break;
	}
	// Get queue
	vkGetDeviceQueue(vkDevice, graphicsFamily, 0, &vkGraphicsQueue);
	vkGetDeviceQueue(vkDevice, presentFamily, 0, &vkPresentQueue);
/*** Swap chain ***/
	// Get surface format
	uint32_t surfaceFormatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhyDevice, vkSurface, &surfaceFormatCount, nullptr);
	VkSurfaceFormatKHR surfaceFormats[surfaceFormatCount];
	vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhyDevice, vkSurface, &surfaceFormatCount, surfaceFormats);
	// Pick surface format
	if (surfaceFormatCount == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED) {
		vkSurfaceFormat.format = VK_FORMAT_B8G8R8A8_SRGB;
		vkSurfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	}else{
		vkSurfaceFormat.format = surfaceFormats[0].format;
		vkSurfaceFormat.colorSpace = surfaceFormats[0].colorSpace;
		for(uint32_t i = 0; i < surfaceFormatCount; ++i){
			if(surfaceFormats[i].format == VK_FORMAT_A8B8G8R8_SRGB_PACK32 || surfaceFormats[i].format == VK_FORMAT_R8G8B8A8_SRGB || surfaceFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB){
				vkSurfaceFormat.format = surfaceFormats[i].format;
				vkSurfaceFormat.colorSpace = surfaceFormats[i].colorSpace;
				break;
			}
		}
	}
	// Get present mode
	uint32_t presentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhyDevice, vkSurface, &presentModeCount, nullptr);
	
}