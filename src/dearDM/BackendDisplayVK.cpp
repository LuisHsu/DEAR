#include <BackendDisplayVK.hpp>

BackendDisplayVK::BackendDisplayVK():
	BackendBase(BackendBase::SurfType::DISPLAY)
{
	VkPhysicalDevice vkPhyDevice;
	VkDisplayKHR vkDisplay;
	VkDisplayPropertiesKHR vkDisplayProperties;
	VkDisplayModeKHR vkDisplayMode;
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
			vkDisplayExtent = visibleRegion;
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
	swapChainCreateInfo.minImageCount = (surfaceCapability.minImageCount + 1 > surfaceCapability.maxImageCount) ? surfaceCapability.maxImageCount : surfaceCapability.minImageCount + 1;
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
			throw "[Vulkan swapchain] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan swapchain] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		case VK_ERROR_DEVICE_LOST:
			throw "[Vulkan swapchain] VK_ERROR_DEVICE_LOST";
		break;
		case VK_ERROR_SURFACE_LOST_KHR:
			throw "[Vulkan swapchain] VK_ERROR_SURFACE_LOST_KHR";
		break;
		case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
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
	poolInfo.queueFamilyIndex = graphicsFamily;
	poolInfo.flags = 0;
	switch(vkCreateCommandPool(vkDevice, &poolInfo, nullptr, &vkCommandPool)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan command pool] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
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
			throw "[Vulkan command buffer] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan command buffer] VK_ERROR_OUT_OF_DEVICE_MEMORY";
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
		vkCmdEndRenderPass(vkCommandBuffers[i]);
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
/*** Create semaphores ***/
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	switch(vkCreateSemaphore(vkDevice, &semaphoreInfo, nullptr, &vkImageAvailableSemaphore)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan image available semaphore] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan image available semaphore] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		default:
		break;
	}
	switch(vkCreateSemaphore(vkDevice, &semaphoreInfo, nullptr, &vkRenderFinishedSemaphore)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan render finished semaphore] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan render finished semaphore] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		default:
		break;
	}
}

BackendDisplayVK::~BackendDisplayVK(){
	vkDestroySemaphore(vkDevice, vkImageAvailableSemaphore, nullptr);
    vkDestroySemaphore(vkDevice, vkRenderFinishedSemaphore, nullptr);
	vkDestroyCommandPool(vkDevice, vkCommandPool, nullptr);
	vkDestroySwapchainKHR(vkDevice, vkSwapChain, nullptr);
	vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
	vkDestroyDevice(vkDevice, nullptr);
	vkDestroyInstance(vkInstance, nullptr);
}

void BackendDisplayVK::paint(IPCFrameMessage *message){
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

void BackendDisplayVK::initTexture(){
}