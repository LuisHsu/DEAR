#include <BackendXcbVK.hpp>

BackendXcbVK::BackendXcbVK():
	BackendBase(BackendBase::SurfType::XCB)
{
/*== XCB ==*/
	xcb_screen_t *xcbScreen = nullptr;
/*** Connection ***/
	xcbConnection = xcb_connect(nullptr, nullptr);
	switch(xcb_connection_has_error(xcbConnection)){
		case XCB_CONN_ERROR:
			xcb_disconnect(xcbConnection);
			throw "[Vulkan XCB connect] Socket errors, pipe errors or other stream errors.";
		break;
		case XCB_CONN_CLOSED_EXT_NOTSUPPORTED:
			xcb_disconnect(xcbConnection);
			throw "[Vulkan XCB connect] Extension not supported.";
		break;
		case XCB_CONN_CLOSED_MEM_INSUFFICIENT:
			xcb_disconnect(xcbConnection);
			throw "[Vulkan XCB connect] Memory not available.";
		break;
		case XCB_CONN_CLOSED_REQ_LEN_EXCEED:
			xcb_disconnect(xcbConnection);
			throw "[Vulkan XCB connect] Exceeding request length that server accepts.";
		break;
		case XCB_CONN_CLOSED_PARSE_ERR:
			xcb_disconnect(xcbConnection);
			throw "[Vulkan XCB connect] Error during parsing display string.";
		break;
		case XCB_CONN_CLOSED_INVALID_SCREEN:
			xcb_disconnect(xcbConnection);
			throw "[Vulkan XCB connect] Server does not have a screen matching the display.";
		break;
		default:
		break;
	}
/*** Window ***/
	// Get screen
	xcb_screen_iterator_t screenIter = xcb_setup_roots_iterator(xcb_get_setup(xcbConnection));
	xcbScreen = screenIter.data;

	// Create Window
	xcbWindow = xcb_generate_id(xcbConnection);
	xcb_create_window(xcbConnection,
		XCB_COPY_FROM_PARENT,
		xcbWindow,
		xcbScreen->root,
		0, 0,
		xcbScreen->width_in_pixels, xcbScreen->height_in_pixels,
		0,
		XCB_WINDOW_CLASS_INPUT_OUTPUT,
		xcbScreen->root_visual,
		0, NULL
	);
	// Set window property
	const char *windowTitle = "DEAR Desktop";
	xcb_change_property (xcbConnection,
		XCB_PROP_MODE_REPLACE,
		xcbWindow,
		XCB_ATOM_WM_NAME,
		XCB_ATOM_STRING,
		8,
		strlen(windowTitle),
		windowTitle
	);
	// Map window
	xcb_map_window(xcbConnection, xcbWindow);
	xcb_flush(xcbConnection);

/*== Vulkan ==*/
	VkPresentModeKHR vkPresentMode;
/*** Instance ***/
	// App info
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "DearDM";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;
	// Create info
	VkInstanceCreateInfo instanceCreateInfo = {};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pApplicationInfo = &appInfo;
	instanceCreateInfo.enabledLayerCount = 1;
	const char *layerNames[1];
	layerNames[0] = "VK_LAYER_LUNARG_standard_validation";
	instanceCreateInfo.ppEnabledLayerNames = layerNames;
	// Extensions
	instanceCreateInfo.enabledExtensionCount = 4;
	const char *extensionNames[instanceCreateInfo.enabledExtensionCount];
	extensionNames[0] = "VK_KHR_surface";
	extensionNames[1] = "VK_KHR_xcb_surface";
	extensionNames[2] = "VK_EXT_debug_report";
	extensionNames[3] = "VK_KHR_external_memory_capabilities";
	instanceCreateInfo.ppEnabledExtensionNames = extensionNames;
	// Create instance
	switch (vkCreateInstance(&instanceCreateInfo, nullptr, &vkInstance)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			xcb_destroy_window(xcbConnection, xcbWindow);
			xcb_disconnect(xcbConnection);
			throw "[Vulkan instance] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			xcb_destroy_window(xcbConnection, xcbWindow);
			xcb_disconnect(xcbConnection);
			throw "[Vulkan instance] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		case VK_ERROR_INITIALIZATION_FAILED:
			xcb_destroy_window(xcbConnection, xcbWindow);
			xcb_disconnect(xcbConnection);
			throw "[Vulkan instance] VK_ERROR_INITIALIZATION_FAILED";
		break;
		case VK_ERROR_LAYER_NOT_PRESENT:
			xcb_destroy_window(xcbConnection, xcbWindow);
			xcb_disconnect(xcbConnection);
			throw "[Vulkan instance] VK_ERROR_LAYER_NOT_PRESENT";
		break;
		case VK_ERROR_EXTENSION_NOT_PRESENT:
			xcb_destroy_window(xcbConnection, xcbWindow);
			xcb_disconnect(xcbConnection);
			throw "[Vulkan instance] VK_ERROR_EXTENSION_NOT_PRESENT";
		break;
		case VK_ERROR_INCOMPATIBLE_DRIVER:
			xcb_destroy_window(xcbConnection, xcbWindow);
			xcb_disconnect(xcbConnection);
			throw "[Vulkan instance] VK_ERROR_INCOMPATIBLE_DRIVER";
		break;
		default:
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
		}
	}
	// Throw error if no device
	if(bestDevice == -1){
		vkDestroyInstance(vkInstance, nullptr);	
		xcb_destroy_window(xcbConnection, xcbWindow);
		xcb_disconnect(xcbConnection);
		throw "[Vulkan physical device] No suitable device.";
	}
/*** Surface ***/
	// Get display extent
	xcb_get_geometry_reply_t *windowGeometry = xcb_get_geometry_reply(xcbConnection, xcb_get_geometry_unchecked(xcbConnection, xcbWindow), nullptr);
	vkDisplayExtent.width = windowGeometry->width;
	vkDisplayExtent.height = windowGeometry->height;
	// Create surface
	VkXcbSurfaceCreateInfoKHR surfaceCreateInfo = {};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.connection = xcbConnection;
	surfaceCreateInfo.window = xcbWindow;
	switch(vkCreateXcbSurfaceKHR(vkInstance, &surfaceCreateInfo, nullptr, &vkSurface)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
			vkDestroyInstance(vkInstance, nullptr);
			xcb_destroy_window(xcbConnection, xcbWindow);
			xcb_disconnect(xcbConnection);
			throw "[Vulkan surface] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
			vkDestroyInstance(vkInstance, nullptr);
			xcb_destroy_window(xcbConnection, xcbWindow);
			xcb_disconnect(xcbConnection);
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
		presentSupport &= vkGetPhysicalDeviceXcbPresentationSupportKHR(vkPhyDevice, i, xcbConnection, xcbScreen->root_visual);
		if(queueFamilies[i].queueCount > 0 && presentSupport) {
			presentFamily = i;
		}
	}
	if(graphicsFamily < 0 || presentFamily < 0){
		vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
		vkDestroyInstance(vkInstance, nullptr);
		xcb_destroy_window(xcbConnection, xcbWindow);
		xcb_disconnect(xcbConnection);
		throw "[Vulkan queue family] No suitable queue families.";
	}
/*** Logical device ***/
	// Graphics queue info
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	for(int i = 0; i < 2; ++i){
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueCount = 1;
		float queuePriority = 1.0f;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}
	queueCreateInfos[0].queueFamilyIndex = graphicsFamily;
	queueCreateInfos[1].queueFamilyIndex = presentFamily;
	// Create logical device
	VkPhysicalDeviceFeatures deviceFeatures = {};
	VkDeviceCreateInfo logicalDeviceCreateInfo = {};
	logicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	logicalDeviceCreateInfo.queueCreateInfoCount = 2;
	logicalDeviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	logicalDeviceCreateInfo.pEnabledFeatures = &deviceFeatures;
	logicalDeviceCreateInfo.enabledExtensionCount = 1;
	const char *devExtensionNames[logicalDeviceCreateInfo.enabledExtensionCount];
	devExtensionNames[0] = "VK_KHR_swapchain";
	logicalDeviceCreateInfo.ppEnabledExtensionNames = devExtensionNames;
	switch(vkCreateDevice(vkPhyDevice, &logicalDeviceCreateInfo, nullptr, &vkDevice)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			vkDestroyDevice(vkDevice, nullptr);
			vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
			vkDestroyInstance(vkInstance, nullptr);
			xcb_destroy_window(xcbConnection, xcbWindow);
			xcb_disconnect(xcbConnection);
			throw "[Vulkan device] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			vkDestroyDevice(vkDevice, nullptr);
			vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
			vkDestroyInstance(vkInstance, nullptr);
			xcb_destroy_window(xcbConnection, xcbWindow);
			xcb_disconnect(xcbConnection);
			throw "[Vulkan device] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		case VK_ERROR_INITIALIZATION_FAILED:
		vkDestroyDevice(vkDevice, nullptr);
		vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
		vkDestroyInstance(vkInstance, nullptr);
		xcb_destroy_window(xcbConnection, xcbWindow);
		xcb_disconnect(xcbConnection);
			throw "[Vulkan device] VK_ERROR_INITIALIZATION_FAILED";
		break;
		case VK_ERROR_EXTENSION_NOT_PRESENT:
		vkDestroyDevice(vkDevice, nullptr);
		vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
		vkDestroyInstance(vkInstance, nullptr);
		xcb_destroy_window(xcbConnection, xcbWindow);
		xcb_disconnect(xcbConnection);
			throw "[Vulkan device] VK_ERROR_EXTENSION_NOT_PRESENT";
		break;
		case VK_ERROR_FEATURE_NOT_PRESENT:
		vkDestroyDevice(vkDevice, nullptr);
		vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
		vkDestroyInstance(vkInstance, nullptr);
		xcb_destroy_window(xcbConnection, xcbWindow);
		xcb_disconnect(xcbConnection);
			throw "[Vulkan device] VK_ERROR_FEATURE_NOT_PRESENT";
		break;
		case VK_ERROR_TOO_MANY_OBJECTS:
		vkDestroyDevice(vkDevice, nullptr);
		vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
		vkDestroyInstance(vkInstance, nullptr);
		xcb_destroy_window(xcbConnection, xcbWindow);
		xcb_disconnect(xcbConnection);
			throw "[Vulkan device] VK_ERROR_TOO_MANY_OBJECTS";
		break;
		case VK_ERROR_DEVICE_LOST:
		vkDestroyDevice(vkDevice, nullptr);
		vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
		vkDestroyInstance(vkInstance, nullptr);
		xcb_destroy_window(xcbConnection, xcbWindow);
		xcb_disconnect(xcbConnection);
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
	VkPresentModeKHR presentModes[presentModeCount];
	vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhyDevice, vkSurface, &presentModeCount, presentModes);
	for(uint32_t i = 0; i < presentModeCount; ++i){
		if(presentModes[i] == VK_PRESENT_MODE_FIFO_KHR){
			vkPresentMode = VK_PRESENT_MODE_FIFO_KHR;
			break;
		}else if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            vkPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
        }else if (presentModes[i] != VK_PRESENT_MODE_IMMEDIATE_KHR) {
            vkPresentMode = presentModes[i];
        }
	}
	// Swapchain capability
	VkSurfaceCapabilitiesKHR surfaceCapability;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkPhyDevice, vkSurface, &surfaceCapability);
	// Create swap chain
	VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
	swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapChainCreateInfo.surface = vkSurface;
	swapChainCreateInfo.minImageCount = (surfaceCapability.minImageCount > surfaceCapability.maxImageCount + 1) ? surfaceCapability.maxImageCount : surfaceCapability.minImageCount + 1;
	swapChainCreateInfo.imageFormat = vkSurfaceFormat.format;
	swapChainCreateInfo.imageColorSpace = vkSurfaceFormat.colorSpace;
	swapChainCreateInfo.imageExtent = vkDisplayExtent;
	swapChainCreateInfo.imageArrayLayers = 1;
	swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapChainCreateInfo.queueFamilyIndexCount = 0;
	swapChainCreateInfo.pQueueFamilyIndices = nullptr;
	swapChainCreateInfo.preTransform = surfaceCapability.currentTransform;
	swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapChainCreateInfo.presentMode = vkPresentMode;
	swapChainCreateInfo.clipped = VK_TRUE;
	swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
	switch(vkCreateSwapchainKHR(vkDevice, &swapChainCreateInfo, nullptr, &vkSwapChain)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
		vkDestroySwapchainKHR(vkDevice, vkSwapChain, nullptr);
		vkDestroyDevice(vkDevice, nullptr);
		vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
		vkDestroyInstance(vkInstance, nullptr);
		xcb_destroy_window(xcbConnection, xcbWindow);
		xcb_disconnect(xcbConnection);
			throw "[Vulkan swapchain] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
		vkDestroySwapchainKHR(vkDevice, vkSwapChain, nullptr);
		vkDestroyDevice(vkDevice, nullptr);
		vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
		vkDestroyInstance(vkInstance, nullptr);
		xcb_destroy_window(xcbConnection, xcbWindow);
		xcb_disconnect(xcbConnection);
			throw "[Vulkan swapchain] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		case VK_ERROR_DEVICE_LOST:
		vkDestroySwapchainKHR(vkDevice, vkSwapChain, nullptr);
		vkDestroyDevice(vkDevice, nullptr);
		vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
		vkDestroyInstance(vkInstance, nullptr);
		xcb_destroy_window(xcbConnection, xcbWindow);
		xcb_disconnect(xcbConnection);
			throw "[Vulkan swapchain] VK_ERROR_DEVICE_LOST";
		break;
		case VK_ERROR_SURFACE_LOST_KHR:
		vkDestroySwapchainKHR(vkDevice, vkSwapChain, nullptr);
		vkDestroyDevice(vkDevice, nullptr);
		vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
		vkDestroyInstance(vkInstance, nullptr);
		xcb_destroy_window(xcbConnection, xcbWindow);
		xcb_disconnect(xcbConnection);
			throw "[Vulkan swapchain] VK_ERROR_SURFACE_LOST_KHR";
		break;
		case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
		vkDestroySwapchainKHR(vkDevice, vkSwapChain, nullptr);
		vkDestroyDevice(vkDevice, nullptr);
		vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
		vkDestroyInstance(vkInstance, nullptr);
		xcb_destroy_window(xcbConnection, xcbWindow);
		xcb_disconnect(xcbConnection);
			throw "[Vulkan swapchain] VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
		break;
		default:
		break;
	}
	uint32_t swapchainImageCount = 0;
	vkGetSwapchainImagesKHR(vkDevice, vkSwapChain, &swapchainImageCount, nullptr);
	vkSwapChainImages.resize(swapchainImageCount);
	vkGetSwapchainImagesKHR(vkDevice, vkSwapChain, &swapchainImageCount, vkSwapChainImages.data());
/*** Command pool ***/
	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = presentFamily;
	poolInfo.flags = 0;
	switch(vkCreateCommandPool(vkDevice, &poolInfo, nullptr, &vkCommandPool)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
		vkDestroyCommandPool(vkDevice, vkCommandPool, nullptr);
		vkDestroySwapchainKHR(vkDevice, vkSwapChain, nullptr);
		vkDestroyDevice(vkDevice, nullptr);
		vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
		vkDestroyInstance(vkInstance, nullptr);
		xcb_destroy_window(xcbConnection, xcbWindow);
		xcb_disconnect(xcbConnection);
			throw "[Vulkan command pool] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
		vkDestroyCommandPool(vkDevice, vkCommandPool, nullptr);
		vkDestroySwapchainKHR(vkDevice, vkSwapChain, nullptr);
		vkDestroyDevice(vkDevice, nullptr);
		vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
		vkDestroyInstance(vkInstance, nullptr);
		xcb_destroy_window(xcbConnection, xcbWindow);
		xcb_disconnect(xcbConnection);
			throw "[Vulkan command pool] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		default:
		break;
	}
/*** Command buffers ***/
	vkCommandBuffers.resize(swapchainImageCount);
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = vkCommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = swapchainImageCount;
	switch(vkAllocateCommandBuffers(vkDevice, &allocInfo, vkCommandBuffers.data())){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
		vkDestroyCommandPool(vkDevice, vkCommandPool, nullptr);
		vkDestroySwapchainKHR(vkDevice, vkSwapChain, nullptr);
		vkDestroyDevice(vkDevice, nullptr);
		vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
		vkDestroyInstance(vkInstance, nullptr);
		xcb_destroy_window(xcbConnection, xcbWindow);
		xcb_disconnect(xcbConnection);
			throw "[Vulkan command buffer] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
		vkDestroyCommandPool(vkDevice, vkCommandPool, nullptr);
		vkDestroySwapchainKHR(vkDevice, vkSwapChain, nullptr);
		vkDestroyDevice(vkDevice, nullptr);
		vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
		vkDestroyInstance(vkInstance, nullptr);
		xcb_destroy_window(xcbConnection, xcbWindow);
		xcb_disconnect(xcbConnection);
			throw "[Vulkan command buffer] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		default:
		break;
	}
/*** Semaphores ***/
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	switch(vkCreateSemaphore(vkDevice, &semaphoreInfo, nullptr, &vkImageAvailableSemaphore)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
		vkDestroyCommandPool(vkDevice, vkCommandPool, nullptr);
		vkDestroySwapchainKHR(vkDevice, vkSwapChain, nullptr);
		vkDestroyDevice(vkDevice, nullptr);
		vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
		vkDestroyInstance(vkInstance, nullptr);
		xcb_destroy_window(xcbConnection, xcbWindow);
		xcb_disconnect(xcbConnection);
			throw "[Vulkan create semaphore] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
		vkDestroyCommandPool(vkDevice, vkCommandPool, nullptr);
		vkDestroySwapchainKHR(vkDevice, vkSwapChain, nullptr);
		vkDestroyDevice(vkDevice, nullptr);
		vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
		vkDestroyInstance(vkInstance, nullptr);
		xcb_destroy_window(xcbConnection, xcbWindow);
		xcb_disconnect(xcbConnection);
			throw "[Vulkan create semaphore] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		default:
		break;
	}
	switch(vkCreateSemaphore(vkDevice, &semaphoreInfo, nullptr, &vkRenderFinishedSemaphore)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
		vkDestroyCommandPool(vkDevice, vkCommandPool, nullptr);
		vkDestroySwapchainKHR(vkDevice, vkSwapChain, nullptr);
		vkDestroyDevice(vkDevice, nullptr);
		vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
		vkDestroyInstance(vkInstance, nullptr);
		xcb_destroy_window(xcbConnection, xcbWindow);
		xcb_disconnect(xcbConnection);
			throw "[Vulkan create semaphore] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
		vkDestroyCommandPool(vkDevice, vkCommandPool, nullptr);
		vkDestroySwapchainKHR(vkDevice, vkSwapChain, nullptr);
		vkDestroyDevice(vkDevice, nullptr);
		vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
		vkDestroyInstance(vkInstance, nullptr);
		xcb_destroy_window(xcbConnection, xcbWindow);
		xcb_disconnect(xcbConnection);
			throw "[Vulkan create semaphore] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		default:
		break;
	}
/*** Create fence ***/
	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	switch(vkCreateFence(vkDevice, &fenceCreateInfo, nullptr, &vkMapTextureFence)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan create fence] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan create fence] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		default:
		break;
	}
}

BackendXcbVK::~BackendXcbVK(){
	vkDestroySemaphore(vkDevice, vkImageAvailableSemaphore, nullptr);
    vkDestroySemaphore(vkDevice, vkRenderFinishedSemaphore, nullptr);
	vkDestroyCommandPool(vkDevice, vkCommandPool, nullptr);
	vkDestroySwapchainKHR(vkDevice, vkSwapChain, nullptr);
	vkDestroyDevice(vkDevice, nullptr);
	vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
	vkDestroyInstance(vkInstance, nullptr);

	xcb_destroy_window(xcbConnection, xcbWindow);
	xcb_disconnect(xcbConnection);
}

void BackendXcbVK::paint(IPCFrameMessage *message){
	uint32_t imageIndex;
	vkAcquireNextImageKHR(vkDevice, vkSwapChain, std::numeric_limits<uint64_t>::max(), vkImageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
	// Wait for transfer fence
	if(vkWaitForFences(vkDevice, 1, &vkMapTextureFence, VK_TRUE, 3000000000) == VK_SUCCESS){
		// Reset fence
		switch(vkResetFences(vkDevice, 1, &vkMapTextureFence)){
			case VK_ERROR_OUT_OF_HOST_MEMORY:
				throw "[Vulkan reset fence] VK_ERROR_OUT_OF_HOST_MEMORY";
			break;
			case VK_ERROR_OUT_OF_DEVICE_MEMORY:
				throw "[Vulkan reset fence] VK_ERROR_OUT_OF_DEVICE_MEMORY";
			break;
			default:
			break;
		}
		// Map memory
		char *memPtr = nullptr;
		switch(vkMapMemory(vkDevice, vkTextureMemory, 0, memSize, 0, (void **)&memPtr)){
			case VK_ERROR_OUT_OF_HOST_MEMORY:
				throw "[Vulkan map memory] VK_ERROR_OUT_OF_HOST_MEMORY";
			break;
			case VK_ERROR_OUT_OF_DEVICE_MEMORY:
				throw "[Vulkan map memory] VK_ERROR_OUT_OF_DEVICE_MEMORY";
			break;
			case VK_ERROR_MEMORY_MAP_FAILED:
				throw "[Vulkan map memory] VK_ERROR_MEMORY_MAP_FAILED";
			break;
			default:
			break;
		}
		// Copy memory
		memcpy(memPtr, message->dataBytes, memSize);
		// Unmap memory
		vkUnmapMemory(vkDevice, vkTextureMemory);
	}
	// Submit command buffer
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_TRANSFER_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &vkImageAvailableSemaphore;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &vkCommandBuffers[imageIndex];
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &vkRenderFinishedSemaphore;
	switch(vkQueueSubmit(vkGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan queue submit] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan queue submit] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		case VK_ERROR_DEVICE_LOST:
			throw "[Vulkan queue submit] VK_ERROR_DEVICE_LOST";
		break;
		default:
		break;
	}
	// Present
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &vkRenderFinishedSemaphore;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &vkSwapChain;
	presentInfo.pImageIndices = &imageIndex;
	
	switch(vkQueuePresentKHR(vkPresentQueue, &presentInfo)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan queue present] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan queue present] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		case VK_ERROR_DEVICE_LOST:
			throw "[Vulkan queue present] VK_ERROR_DEVICE_LOST";
		break;
		case VK_ERROR_OUT_OF_DATE_KHR:
			throw "[Vulkan queue present] VK_ERROR_OUT_OF_DATE_KHR";
		break;
		case VK_ERROR_SURFACE_LOST_KHR:
			throw "[Vulkan queue present] VK_ERROR_SURFACE_LOST_KHR";
		break;
		default:
		break;
	}	
}
void BackendXcbVK::initTexture(){
/*** Texture image ***/
	// Image crate info
	VkImageCreateInfo textureImageInfo = {};
	textureImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	textureImageInfo.flags = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
	textureImageInfo.imageType = VK_IMAGE_TYPE_2D;
	textureImageInfo.format = vkSurfaceFormat.format;
	textureImageInfo.extent.width = vkDisplayExtent.width;
	textureImageInfo.extent.height = vkDisplayExtent.height;
	textureImageInfo.extent.depth = 1;
	textureImageInfo.mipLevels = 1;
	textureImageInfo.arrayLayers = 1;
	textureImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	textureImageInfo.tiling = VK_IMAGE_TILING_LINEAR;
	textureImageInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	textureImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	textureImageInfo.queueFamilyIndexCount = 0;
	textureImageInfo.pQueueFamilyIndices = nullptr;
	textureImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	switch(vkCreateImage(vkDevice, &textureImageInfo, nullptr, &vkTextureImage)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan create texture image] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan create texture image] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		default:
		break;
	}
/*** Texture memory ***/
	// Get requirement
	VkMemoryRequirements textureMemoryRequirement = {};
	vkGetImageMemoryRequirements(vkDevice, vkTextureImage, &textureMemoryRequirement);
	// Allocate info
	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.allocationSize = textureMemoryRequirement.size;
	memSize = textureMemoryRequirement.size;
	memoryAllocateInfo.memoryTypeIndex = findMemoryType(textureMemoryRequirement.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	// Allocate
	switch(vkAllocateMemory(vkDevice, &memoryAllocateInfo, nullptr, &vkTextureMemory)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan allocate texture memory] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan allocate texture memory] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		case VK_ERROR_TOO_MANY_OBJECTS:
			throw "[Vulkan allocate texture memory] VK_ERROR_TOO_MANY_OBJECTS";
		break;
		case VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR:
			throw "[Vulkan allocate texture memory] VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR";
		break;
		default:
		break;
	}
	// Bind to image
	switch(vkBindImageMemory(vkDevice, vkTextureImage, vkTextureMemory, 0)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan bind texture memory] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan bind texture memory] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		default:
		break;
	}



/*** Command buffer record ***/
	for(uint32_t i = 0; i < vkCommandBuffers.size(); ++i) {
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		beginInfo.pInheritanceInfo = nullptr;
		// Start command buffer
		vkBeginCommandBuffer(vkCommandBuffers[i], &beginInfo);
		// Image barrier
		VkImageSubresourceRange subresourceRange = {};
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.levelCount = 1;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.layerCount = 1;
		VkImageMemoryBarrier imageMemoryBarrier = {};
		imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
		imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.image = vkTextureImage;
		imageMemoryBarrier.subresourceRange = subresourceRange;
		vkCmdPipelineBarrier(vkCommandBuffers[i],
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_DEPENDENCY_BY_REGION_BIT,
			0, nullptr,
			0, nullptr,
			1, &imageMemoryBarrier
		);
		imageMemoryBarrier.image = vkSwapChainImages[i];
		vkCmdPipelineBarrier(vkCommandBuffers[i],
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_DEPENDENCY_BY_REGION_BIT,
			0, nullptr,
			0, nullptr,
			1, &imageMemoryBarrier
		);
		// Copy image
		VkImageSubresourceLayers subResourceLayer = {};
		subResourceLayer.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subResourceLayer.mipLevel = 0;
		subResourceLayer.baseArrayLayer = 0;
		subResourceLayer.layerCount = 1;
		VkOffset3D imageCopyOffset = {};
		imageCopyOffset.x = 0;
		imageCopyOffset.y = 0;
		imageCopyOffset.z = 0;
		VkExtent3D imageCopyExtent = {};
		imageCopyExtent.width = vkDisplayExtent.width;
		imageCopyExtent.height = vkDisplayExtent.height;
		imageCopyExtent.depth = 1;
		VkImageCopy imageCopy = {};
		imageCopy.srcSubresource = subResourceLayer;
		imageCopy.srcOffset = imageCopyOffset;
		imageCopy.dstSubresource = subResourceLayer;
		imageCopy.dstOffset = imageCopyOffset;
		imageCopy.extent = imageCopyExtent;
		vkCmdCopyImage(vkCommandBuffers[i],
			vkTextureImage,
			VK_IMAGE_LAYOUT_GENERAL,
			vkSwapChainImages[i],
			VK_IMAGE_LAYOUT_GENERAL,
			1,
			&imageCopy
		);
		// Change layout to present
		imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
		imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		vkCmdPipelineBarrier(vkCommandBuffers[i],
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_DEPENDENCY_BY_REGION_BIT,
			0, nullptr,
			0, nullptr,
			1, &imageMemoryBarrier
		);
		// End command
		switch(vkEndCommandBuffer(vkCommandBuffers[i])){
			case VK_ERROR_OUT_OF_HOST_MEMORY:
				throw "[Vulkan command buffer record] VK_ERROR_OUT_OF_HOST_MEMORY";
			break;
			case VK_ERROR_OUT_OF_DEVICE_MEMORY:
				throw "[Vulkan command buffer record] VK_ERROR_OUT_OF_DEVICE_MEMORY";
			break;
			default:
			break;
		}
	}
}