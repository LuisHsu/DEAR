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
	VkPhysicalDevice vkPhyDevice;
	VkPresentModeKHR vkPresentMode;
	VkShaderModule vkVertexShader;
	VkShaderModule vkFragmentShader;
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
	instanceCreateInfo.enabledLayerCount = 0;
	// Extensions
	instanceCreateInfo.enabledExtensionCount = 2;
	const char *extensionNames[instanceCreateInfo.enabledExtensionCount];
	extensionNames[0] = "VK_KHR_surface";
	extensionNames[1] = "VK_KHR_xcb_surface";
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
	swapChainCreateInfo.minImageCount = (surfaceCapability.minImageCount > surfaceCapability.maxImageCount) ? surfaceCapability.maxImageCount : surfaceCapability.minImageCount;
	swapChainCreateInfo.imageFormat = vkSurfaceFormat.format;
	swapChainCreateInfo.imageColorSpace = vkSurfaceFormat.colorSpace;
	swapChainCreateInfo.imageExtent = vkDisplayExtent;
	swapChainCreateInfo.imageArrayLayers = 1;
	swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	if (graphicsFamily != presentFamily) {
		swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapChainCreateInfo.queueFamilyIndexCount = 2;
		uint32_t queueFamilyIndices[] = {(uint32_t)graphicsFamily, (uint32_t)presentFamily};
		swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
	} else {
		swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapChainCreateInfo.queueFamilyIndexCount = 0;
		swapChainCreateInfo.pQueueFamilyIndices = nullptr;
	}
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
/*** Image View ***/
	vkSwapChainImageViews.resize(swapchainImageCount);	
	for(uint32_t i = 0; i < swapchainImageCount; ++i){
		VkImageViewCreateInfo imageViewCreateInfo = {};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.image = vkSwapChainImages[i];
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format = vkSurfaceFormat.format;
		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;
		if(vkCreateImageView(vkDevice, &imageViewCreateInfo, nullptr, &vkSwapChainImageViews[i])!= VK_SUCCESS){
			vkDestroyImageView(vkDevice, vkSwapChainImageViews[i], nullptr);
			vkDestroySwapchainKHR(vkDevice, vkSwapChain, nullptr);
			vkDestroyDevice(vkDevice, nullptr);
			vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
			vkDestroyInstance(vkInstance, nullptr);
			xcb_destroy_window(xcbConnection, xcbWindow);
			xcb_disconnect(xcbConnection);
			throw "[Vulkan image view] Error create image view.";
		}
	}
/*** Shader ***/
	// Vertex shader
	vkVertexShader = createShaderModule("vert/BackendVK.vert.spv");
	VkPipelineShaderStageCreateInfo vertexShaderStageCreateInfo = {};
	vertexShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertexShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertexShaderStageCreateInfo.module = vkVertexShader;
	vertexShaderStageCreateInfo.pName = "main";
	vkFragmentShader = createShaderModule("frag/BackendVK.frag.spv");
	VkPipelineShaderStageCreateInfo fragmentShaderStageCreateInfo = {};
	fragmentShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragmentShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragmentShaderStageCreateInfo.module = vkFragmentShader;
	fragmentShaderStageCreateInfo.pName = "main";
/*** Fixed functions ***/
	// Vertex input
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 0;
	vertexInputInfo.pVertexBindingDescriptions = nullptr;
	vertexInputInfo.vertexAttributeDescriptionCount = 0;
	vertexInputInfo.pVertexAttributeDescriptions = nullptr;
	// Input assembly
	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;
	// Viewport
	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float) vkDisplayExtent.width;
	viewport.height = (float) vkDisplayExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	// Scissor
	VkRect2D scissor = {};
	scissor.offset = {0, 0};
	scissor.extent = vkDisplayExtent;
	// Viewport state
	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;
	// Rasterizer
	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f;
	rasterizer.depthBiasClamp = 0.0f;
	rasterizer.depthBiasSlopeFactor = 0.0f;
	// Multi sampling
	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f;
	multisampling.pSampleMask = nullptr;
	multisampling.alphaToCoverageEnable = VK_FALSE;
	multisampling.alphaToOneEnable = VK_FALSE;
	// Color blend
	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;
/*** Render pass ***/
	// Color attachment
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = vkSurfaceFormat.format;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	// Color attachment ref
	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	// Subpass description
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	// Subpass dependency
	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	// Render pass
	VkRenderPassCreateInfo renderPassCreateInfo = {};
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.attachmentCount = 1;
	renderPassCreateInfo.pAttachments = &colorAttachment;
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpass;
	renderPassCreateInfo.dependencyCount = 1;
	renderPassCreateInfo.pDependencies = &dependency;
	switch(vkCreateRenderPass(vkDevice, &renderPassCreateInfo, nullptr, &vkRenderPass)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			vkDestroyRenderPass(vkDevice, vkRenderPass, nullptr);
			for (VkImageView imageView : vkSwapChainImageViews) {
				vkDestroyImageView(vkDevice, imageView, nullptr);
			}
			vkDestroySwapchainKHR(vkDevice, vkSwapChain, nullptr);
			vkDestroyDevice(vkDevice, nullptr);
			vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
			vkDestroyInstance(vkInstance, nullptr);
			xcb_destroy_window(xcbConnection, xcbWindow);
			xcb_disconnect(xcbConnection);
			throw "[Vulkan render pass] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			vkDestroyRenderPass(vkDevice, vkRenderPass, nullptr);
			for (VkImageView imageView : vkSwapChainImageViews) {
				vkDestroyImageView(vkDevice, imageView, nullptr);
			}
			vkDestroySwapchainKHR(vkDevice, vkSwapChain, nullptr);
			vkDestroyDevice(vkDevice, nullptr);
			vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
			vkDestroyInstance(vkInstance, nullptr);
			xcb_destroy_window(xcbConnection, xcbWindow);
			xcb_disconnect(xcbConnection);
			throw "[Vulkan render pass] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		default:
		break;
	}
/*** Graphics pipeline ***/
	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	VkPipelineShaderStageCreateInfo shaderStages[] = {vertexShaderStageCreateInfo, fragmentShaderStageCreateInfo};
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = nullptr;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = nullptr;
	pipelineInfo.renderPass = vkRenderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -1;
	switch(vkCreateGraphicsPipelines(vkDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &vkGraphicsPipeline)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
		vkDestroyRenderPass(vkDevice, vkRenderPass, nullptr);
		for (VkImageView imageView : vkSwapChainImageViews) {
			vkDestroyImageView(vkDevice, imageView, nullptr);
		}
		vkDestroySwapchainKHR(vkDevice, vkSwapChain, nullptr);
		vkDestroyDevice(vkDevice, nullptr);
		vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
		vkDestroyInstance(vkInstance, nullptr);
		xcb_destroy_window(xcbConnection, xcbWindow);
		xcb_disconnect(xcbConnection);
			throw "[Vulkan pipeline] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
		vkDestroyRenderPass(vkDevice, vkRenderPass, nullptr);
		for (VkImageView imageView : vkSwapChainImageViews) {
			vkDestroyImageView(vkDevice, imageView, nullptr);
		}
		vkDestroySwapchainKHR(vkDevice, vkSwapChain, nullptr);
		vkDestroyDevice(vkDevice, nullptr);
		vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
		vkDestroyInstance(vkInstance, nullptr);
		xcb_destroy_window(xcbConnection, xcbWindow);
		xcb_disconnect(xcbConnection);
			throw "[Vulkan pipeline] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		case VK_ERROR_INVALID_SHADER_NV:
		vkDestroyRenderPass(vkDevice, vkRenderPass, nullptr);
		for (VkImageView imageView : vkSwapChainImageViews) {
			vkDestroyImageView(vkDevice, imageView, nullptr);
		}
		vkDestroySwapchainKHR(vkDevice, vkSwapChain, nullptr);
		vkDestroyDevice(vkDevice, nullptr);
		vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
		vkDestroyInstance(vkInstance, nullptr);
		xcb_destroy_window(xcbConnection, xcbWindow);
		xcb_disconnect(xcbConnection);
			throw "[Vulkan pipeline] VK_ERROR_INVALID_SHADER_NV";
		break;
		default:
		break;
	}
	vkDestroyShaderModule(vkDevice, vkVertexShader, nullptr);
    vkDestroyShaderModule(vkDevice, vkFragmentShader, nullptr);
/*** Frame buffers ***/
	vkSwapChainFramebuffers.resize(swapchainImageCount);
	for (size_t i = 0; i < swapchainImageCount; ++i) {
		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = vkRenderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = &vkSwapChainImageViews[i];
		framebufferInfo.width = vkDisplayExtent.width;
		framebufferInfo.height = vkDisplayExtent.height;
		framebufferInfo.layers = 1;
		switch(vkCreateFramebuffer(vkDevice, &framebufferInfo, nullptr, &vkSwapChainFramebuffers[i])){
			case VK_ERROR_OUT_OF_HOST_MEMORY:
			vkDestroyFramebuffer(vkDevice, vkSwapChainFramebuffers[i], nullptr);
			vkDestroyRenderPass(vkDevice, vkRenderPass, nullptr);
			for (VkImageView imageView : vkSwapChainImageViews) {
				vkDestroyImageView(vkDevice, imageView, nullptr);
			}
			vkDestroySwapchainKHR(vkDevice, vkSwapChain, nullptr);
			vkDestroyDevice(vkDevice, nullptr);
			vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
			vkDestroyInstance(vkInstance, nullptr);
			xcb_destroy_window(xcbConnection, xcbWindow);
			xcb_disconnect(xcbConnection);
				throw "[Vulkan framebuffer] VK_ERROR_OUT_OF_HOST_MEMORY";
			break;
			case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			vkDestroyFramebuffer(vkDevice, vkSwapChainFramebuffers[i], nullptr);
			vkDestroyRenderPass(vkDevice, vkRenderPass, nullptr);
			for (VkImageView imageView : vkSwapChainImageViews) {
				vkDestroyImageView(vkDevice, imageView, nullptr);
			}
			vkDestroySwapchainKHR(vkDevice, vkSwapChain, nullptr);
			vkDestroyDevice(vkDevice, nullptr);
			vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
			vkDestroyInstance(vkInstance, nullptr);
			xcb_destroy_window(xcbConnection, xcbWindow);
			xcb_disconnect(xcbConnection);
				throw "[Vulkan framebuffer] VK_ERROR_OUT_OF_DEVICE_MEMORY";
			break;
			default:
			break;
		}
	}

/*** Command pool ***/
	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = graphicsFamily;
	poolInfo.flags = 0;
	switch(vkCreateCommandPool(vkDevice, &poolInfo, nullptr, &vkCommandPool)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
		vkDestroyCommandPool(vkDevice, vkCommandPool, nullptr);
		for (VkFramebuffer framebuffer : vkSwapChainFramebuffers) {
			vkDestroyFramebuffer(vkDevice, framebuffer, nullptr);
		}
		vkDestroyRenderPass(vkDevice, vkRenderPass, nullptr);
		for (VkImageView imageView : vkSwapChainImageViews) {
			vkDestroyImageView(vkDevice, imageView, nullptr);
		}
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
		for (VkFramebuffer framebuffer : vkSwapChainFramebuffers) {
			vkDestroyFramebuffer(vkDevice, framebuffer, nullptr);
		}
		vkDestroyRenderPass(vkDevice, vkRenderPass, nullptr);
		for (VkImageView imageView : vkSwapChainImageViews) {
			vkDestroyImageView(vkDevice, imageView, nullptr);
		}
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
		for (VkFramebuffer framebuffer : vkSwapChainFramebuffers) {
			vkDestroyFramebuffer(vkDevice, framebuffer, nullptr);
		}
		vkDestroyRenderPass(vkDevice, vkRenderPass, nullptr);
		for (VkImageView imageView : vkSwapChainImageViews) {
			vkDestroyImageView(vkDevice, imageView, nullptr);
		}
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
		for (VkFramebuffer framebuffer : vkSwapChainFramebuffers) {
			vkDestroyFramebuffer(vkDevice, framebuffer, nullptr);
		}
		vkDestroyRenderPass(vkDevice, vkRenderPass, nullptr);
		for (VkImageView imageView : vkSwapChainImageViews) {
			vkDestroyImageView(vkDevice, imageView, nullptr);
		}
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

/*** Create semaphores ***/
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	switch(vkCreateSemaphore(vkDevice, &semaphoreInfo, nullptr, &vkImageAvailableSemaphore)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
		vkDestroySemaphore(vkDevice, vkImageAvailableSemaphore, nullptr);
		vkDestroyCommandPool(vkDevice, vkCommandPool, nullptr);
		for (VkFramebuffer framebuffer : vkSwapChainFramebuffers) {
			vkDestroyFramebuffer(vkDevice, framebuffer, nullptr);
		}
		vkDestroyRenderPass(vkDevice, vkRenderPass, nullptr);
		for (VkImageView imageView : vkSwapChainImageViews) {
			vkDestroyImageView(vkDevice, imageView, nullptr);
		}
		vkDestroySwapchainKHR(vkDevice, vkSwapChain, nullptr);
		vkDestroyDevice(vkDevice, nullptr);
		vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
		vkDestroyInstance(vkInstance, nullptr);
		xcb_destroy_window(xcbConnection, xcbWindow);
		xcb_disconnect(xcbConnection);
			throw "[Vulkan image available semaphore] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
		vkDestroySemaphore(vkDevice, vkImageAvailableSemaphore, nullptr);
		vkDestroyCommandPool(vkDevice, vkCommandPool, nullptr);
		for (VkFramebuffer framebuffer : vkSwapChainFramebuffers) {
			vkDestroyFramebuffer(vkDevice, framebuffer, nullptr);
		}
		vkDestroyRenderPass(vkDevice, vkRenderPass, nullptr);
		for (VkImageView imageView : vkSwapChainImageViews) {
			vkDestroyImageView(vkDevice, imageView, nullptr);
		}
		vkDestroySwapchainKHR(vkDevice, vkSwapChain, nullptr);
		vkDestroyDevice(vkDevice, nullptr);
		vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
		vkDestroyInstance(vkInstance, nullptr);
		xcb_destroy_window(xcbConnection, xcbWindow);
		xcb_disconnect(xcbConnection);
			throw "[Vulkan image available semaphore] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		default:
		break;
	}
	switch(vkCreateSemaphore(vkDevice, &semaphoreInfo, nullptr, &vkRenderFinishedSemaphore)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
		vkDestroySemaphore(vkDevice, vkRenderFinishedSemaphore, nullptr);
		vkDestroySemaphore(vkDevice, vkImageAvailableSemaphore, nullptr);
		vkDestroyCommandPool(vkDevice, vkCommandPool, nullptr);
		for (VkFramebuffer framebuffer : vkSwapChainFramebuffers) {
			vkDestroyFramebuffer(vkDevice, framebuffer, nullptr);
		}
		vkDestroyRenderPass(vkDevice, vkRenderPass, nullptr);
		for (VkImageView imageView : vkSwapChainImageViews) {
			vkDestroyImageView(vkDevice, imageView, nullptr);
		}
		vkDestroySwapchainKHR(vkDevice, vkSwapChain, nullptr);
		vkDestroyDevice(vkDevice, nullptr);
		vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
		vkDestroyInstance(vkInstance, nullptr);
		xcb_destroy_window(xcbConnection, xcbWindow);
		xcb_disconnect(xcbConnection);
			throw "[Vulkan render finished semaphore] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
		vkDestroySemaphore(vkDevice, vkRenderFinishedSemaphore, nullptr);
		vkDestroySemaphore(vkDevice, vkImageAvailableSemaphore, nullptr);
		vkDestroyCommandPool(vkDevice, vkCommandPool, nullptr);
		for (VkFramebuffer framebuffer : vkSwapChainFramebuffers) {
			vkDestroyFramebuffer(vkDevice, framebuffer, nullptr);
		}
		vkDestroyRenderPass(vkDevice, vkRenderPass, nullptr);
		for (VkImageView imageView : vkSwapChainImageViews) {
			vkDestroyImageView(vkDevice, imageView, nullptr);
		}
		vkDestroySwapchainKHR(vkDevice, vkSwapChain, nullptr);
		vkDestroyDevice(vkDevice, nullptr);
		vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
		vkDestroyInstance(vkInstance, nullptr);
		xcb_destroy_window(xcbConnection, xcbWindow);
		xcb_disconnect(xcbConnection);
			throw "[Vulkan render finished semaphore] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		default:
		break;
	}

}

BackendXcbVK::~BackendXcbVK(){
	vkDestroySemaphore(vkDevice, vkRenderFinishedSemaphore, nullptr);
	vkDestroySemaphore(vkDevice, vkImageAvailableSemaphore, nullptr);
	vkDestroyCommandPool(vkDevice, vkCommandPool, nullptr);
	for (VkFramebuffer framebuffer : vkSwapChainFramebuffers) {
        vkDestroyFramebuffer(vkDevice, framebuffer, nullptr);
	}
	vkDestroyRenderPass(vkDevice, vkRenderPass, nullptr);
	for (VkImageView imageView : vkSwapChainImageViews) {
        vkDestroyImageView(vkDevice, imageView, nullptr);
    }
	vkDestroySwapchainKHR(vkDevice, vkSwapChain, nullptr);
	vkDestroyDevice(vkDevice, nullptr);
	vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
	vkDestroyInstance(vkInstance, nullptr);

	xcb_destroy_window(xcbConnection, xcbWindow);
	xcb_disconnect(xcbConnection);
}

void BackendXcbVK::paint(){
	uint32_t imageIndex;
	vkAcquireNextImageKHR(vkDevice, vkSwapChain, std::numeric_limits<uint64_t>::max(), vkImageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
	// Submit command buffer
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
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
void BackendXcbVK::initTexture(int fd){
	VkDescriptorSetLayout vkDescriptorSetLayout;
/*** Texture image ***/
	// External image create info
	VkExternalMemoryImageCreateInfoKHR externalImageCreateInfo = {};
	externalImageCreateInfo.sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO_KHR;
	externalImageCreateInfo.handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT_KHR;
	// Image crate info
	VkImageCreateInfo textureImageInfo = {};
	textureImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	textureImageInfo.pNext = &externalImageCreateInfo;
	textureImageInfo.flags = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
	textureImageInfo.imageType = VK_IMAGE_TYPE_2D;
	textureImageInfo.format = vkSurfaceFormat.format;
	textureImageInfo.extent.width = vkDisplayExtent.width;
	textureImageInfo.extent.height = vkDisplayExtent.height;
	textureImageInfo.extent.depth = 1;
	textureImageInfo.mipLevels = 1;
	textureImageInfo.arrayLayers = 1;
	textureImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	textureImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	textureImageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
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
	VkImportMemoryFdInfoKHR importAllocateInfo = {};
	importAllocateInfo.sType = VK_STRUCTURE_TYPE_IMPORT_MEMORY_FD_INFO_KHR;
	importAllocateInfo.handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT_KHR;
	importAllocateInfo.fd = fd;
	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.pNext = &importAllocateInfo;
	memoryAllocateInfo.allocationSize = textureMemoryRequirement.size;
	memoryAllocateInfo.memoryTypeIndex = ffs(textureMemoryRequirement.memoryTypeBits) - 1;
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
/*** Texture image view ***/
	// Creare info
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = vkTextureImage;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = vkSurfaceFormat.format;
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;
	if(vkCreateImageView(vkDevice, &viewInfo, nullptr, &vkTextureImageView)!= VK_SUCCESS){
		throw "[Vulkan texture image view] Error create image view.";
	}
/*** Texture sampler ***/
	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = 16;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;
	if(vkCreateSampler(vkDevice, &samplerInfo, nullptr, &vkTextureSampler)!= VK_SUCCESS){
		throw "[Vulkan texture sampler] Error create sampler.";
	}
/*** Descriptor Set **/
	// Layout binding
	VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
	samplerLayoutBinding.binding = 0;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &samplerLayoutBinding;
	if(vkCreateDescriptorSetLayout(vkDevice, &layoutInfo, nullptr, &vkDescriptorSetLayout) != VK_SUCCESS) {
		throw "[Vulkan texture sampler] Error create descriptor set layout!";
	}
	// Descriptor pool
	VkDescriptorPoolSize poolSize = {};
	poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSize.descriptorCount = 1;
	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = 1;
	if(vkCreateDescriptorPool(vkDevice, &poolInfo, nullptr, &vkDescriptorPool) != VK_SUCCESS) {
		throw "[Vulkan texture descriptor] Error create descriptor pool.";
	}
	// Descriptor set
	VkDescriptorSetAllocateInfo descriptorSetAllocInfo = {};
	descriptorSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocInfo.descriptorPool = vkDescriptorPool;
	descriptorSetAllocInfo.descriptorSetCount = 1;
	descriptorSetAllocInfo.pSetLayouts = &vkDescriptorSetLayout;
	if(vkAllocateDescriptorSets(vkDevice, &descriptorSetAllocInfo, &vkDescriptorSet) != VK_SUCCESS) {
		throw "[Vulkan texture descriptor] Error allocate descriptor set!";
	}
	// Descriptor set image info
	VkDescriptorImageInfo imageInfo = {};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = vkTextureImageView;
	imageInfo.sampler = vkTextureSampler;
	// Write
	VkWriteDescriptorSet descriptorWrite = {};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = vkDescriptorSet;
	descriptorWrite.dstBinding = 1;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pImageInfo = &imageInfo;
	vkUpdateDescriptorSets(vkDevice, 1, &descriptorWrite, 0, nullptr);
/*** Pipeline layout ***/
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pSetLayouts = nullptr;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = 0;
	switch(vkCreatePipelineLayout(vkDevice, &pipelineLayoutInfo, nullptr, &vkPipelineLayout)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			vkDestroyPipelineLayout(vkDevice, vkPipelineLayout, nullptr);
			throw "[Vulkan pipeline layout] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			vkDestroyPipelineLayout(vkDevice, vkPipelineLayout, nullptr);
			throw "[Vulkan pipeline layout] VK_ERROR_OUT_OF_DEVICE_MEMORY";
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
		// Start render pass
		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = vkRenderPass;
		renderPassInfo.framebuffer = vkSwapChainFramebuffers[i];
		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = vkDisplayExtent;
		VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;
		vkCmdBeginRenderPass(vkCommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindDescriptorSets(vkCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipelineLayout, 0, 1, &vkDescriptorSet, 0, nullptr);
		vkCmdBindPipeline(vkCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vkGraphicsPipeline);
		vkCmdDraw(vkCommandBuffers[i], 6, 1, 0, 0);
		vkCmdEndRenderPass(vkCommandBuffers[i]);
		switch(vkEndCommandBuffer(vkCommandBuffers[i])){
			case VK_ERROR_OUT_OF_HOST_MEMORY:
			vkDestroyCommandPool(vkDevice, vkCommandPool, nullptr);
			for (VkFramebuffer framebuffer : vkSwapChainFramebuffers) {
				vkDestroyFramebuffer(vkDevice, framebuffer, nullptr);
			}
			vkDestroyRenderPass(vkDevice, vkRenderPass, nullptr);
			for (VkImageView imageView : vkSwapChainImageViews) {
				vkDestroyImageView(vkDevice, imageView, nullptr);
			}
			vkDestroySwapchainKHR(vkDevice, vkSwapChain, nullptr);
			vkDestroyDevice(vkDevice, nullptr);
			vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
			vkDestroyInstance(vkInstance, nullptr);
			xcb_destroy_window(xcbConnection, xcbWindow);
			xcb_disconnect(xcbConnection);
				throw "[Vulkan command buffer record] VK_ERROR_OUT_OF_HOST_MEMORY";
			break;
			case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			vkDestroyCommandPool(vkDevice, vkCommandPool, nullptr);
			for (VkFramebuffer framebuffer : vkSwapChainFramebuffers) {
				vkDestroyFramebuffer(vkDevice, framebuffer, nullptr);
			}
			vkDestroyRenderPass(vkDevice, vkRenderPass, nullptr);
			for (VkImageView imageView : vkSwapChainImageViews) {
				vkDestroyImageView(vkDevice, imageView, nullptr);
			}
			vkDestroySwapchainKHR(vkDevice, vkSwapChain, nullptr);
			vkDestroyDevice(vkDevice, nullptr);
			vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
			vkDestroyInstance(vkInstance, nullptr);
			xcb_destroy_window(xcbConnection, xcbWindow);
			xcb_disconnect(xcbConnection);
				throw "[Vulkan command buffer record] VK_ERROR_OUT_OF_DEVICE_MEMORY";
			break;
			default:
			break;
		}
	}
}