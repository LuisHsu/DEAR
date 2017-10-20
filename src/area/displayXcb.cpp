#include "displayXcb.hpp"

DisplayXcb::DisplayXcb(IPCServer *server):
	server(server)
{
/*== XCB ==*/
	xcb_screen_t *screenXcb = nullptr;
/*** Connection ***/
	connectionXcb = xcb_connect(nullptr, nullptr);
	switch(xcb_connection_has_error(connectionXcb)){
		case XCB_CONN_ERROR:
			throw "[XCB Display connect] Socket errors, pipe errors or other stream errors.";
		break;
		case XCB_CONN_CLOSED_EXT_NOTSUPPORTED:
			throw "[XCB Display connect] Extension not supported.";
		break;
		case XCB_CONN_CLOSED_MEM_INSUFFICIENT:
			throw "[XCB Display connect] Memory not available.";
		break;
		case XCB_CONN_CLOSED_REQ_LEN_EXCEED:
			throw "[XCB Display connect] Exceeding request length that server accepts.";
		break;
		case XCB_CONN_CLOSED_PARSE_ERR:
			throw "[XCB Display connect] Error during parsing display string.";
		break;
		case XCB_CONN_CLOSED_INVALID_SCREEN:
			throw "[Vulkan XCB connect] Server does not have a screen matching the display.";
		break;
		default:
		break;
	}
/*** Window ***/
	// Get screen
	xcb_screen_iterator_t screenIter = xcb_setup_roots_iterator(xcb_get_setup(connectionXcb));
	screenXcb = screenIter.data;

	// Create Window
	windowXcb = xcb_generate_id(connectionXcb);
	uint32_t eventVal = XCB_EVENT_MASK_EXPOSURE;
	xcb_create_window(connectionXcb,
		XCB_COPY_FROM_PARENT,
		windowXcb,
		screenXcb->root,
		0, 0,
		screenXcb->width_in_pixels, screenXcb->height_in_pixels,
		0,
		XCB_WINDOW_CLASS_INPUT_OUTPUT,
		screenXcb->root_visual,
		XCB_CW_EVENT_MASK, &eventVal
	);
	// Set window property
	const char *windowTitle = "DEAR Desktop";
	xcb_change_property (connectionXcb,
		XCB_PROP_MODE_REPLACE,
		windowXcb,
		XCB_ATOM_WM_NAME,
		XCB_ATOM_STRING,
		8,
		strlen(windowTitle),
		windowTitle
	);
	closeReply = xcb_intern_atom_reply(connectionXcb, xcb_intern_atom(connectionXcb, 0, 16, "WM_DELETE_WINDOW"),0);
	xcb_change_property (connectionXcb,
		XCB_PROP_MODE_REPLACE,
		windowXcb,
		xcb_intern_atom_reply(connectionXcb, xcb_intern_atom(connectionXcb, 1, 12, "WM_PROTOCOLS"),0)->atom,
		XCB_ATOM_ATOM,
		32,
		1,
		&(closeReply->atom)
	);
	// Map window
	xcb_map_window(connectionXcb, windowXcb);
	xcb_flush(connectionXcb);
/*** Events ***/
	// Combined with libuv
	uv_poll_init(server->getLoop(), &xcbPollUv, xcb_get_file_descriptor(connectionXcb));
	xcbPollUv.data = this;
	uv_poll_start(&xcbPollUv, UV_READABLE, [](uv_poll_t* handle, int status, int events){
		DisplayXcb *display = (DisplayXcb *)handle->data;
		display->handleXcbEvent(xcb_poll_for_event(display->connectionXcb));
	});
/*== Vulkan ==*/
/*** Instance ***/
	// App info
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Dear desktop";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;
	// Create info
	VkInstanceCreateInfo instanceCreateInfo = {};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pApplicationInfo = &appInfo;
	instanceCreateInfo.enabledLayerCount = 0;
	//const char *layerNames[1];
	//layerNames[0] = "VK_LAYER_LUNARG_standard_validation";
	//instanceCreateInfo.ppEnabledLayerNames = layerNames;
	// Extensions
	instanceCreateInfo.enabledExtensionCount = 3;
	const char *extensionNames[instanceCreateInfo.enabledExtensionCount];
	extensionNames[0] = "VK_KHR_surface";
	extensionNames[1] = "VK_KHR_xcb_surface";
	extensionNames[2] = "VK_EXT_debug_report";
	//extensionNames[3] = "VK_KHR_external_memory_capabilities";
	instanceCreateInfo.ppEnabledExtensionNames = extensionNames;
	// Create instance
	switch (vkCreateInstance(&instanceCreateInfo, nullptr, &instanceVk)){
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
	vkEnumeratePhysicalDevices(instanceVk, &deviceCount, nullptr);
	VkPhysicalDevice devices[deviceCount];
	vkEnumeratePhysicalDevices(instanceVk, &deviceCount, devices);
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
			phyDeviceVk = devices[i];
		}
	}
	// Throw error if no device
	if(bestDevice == -1){
		throw "[Vulkan physical device] No suitable device.";
	}
/*** Surface ***/
	// Get display extent
	xcb_get_geometry_reply_t *windowGeometry = xcb_get_geometry_reply(connectionXcb, xcb_get_geometry_unchecked(connectionXcb, windowXcb), nullptr);
	displayExtentVk.width = windowGeometry->width;
	displayExtentVk.height = windowGeometry->height;
	// Create surface
	VkXcbSurfaceCreateInfoKHR surfaceCreateInfo = {};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.connection = connectionXcb;
	surfaceCreateInfo.window = windowXcb;
	switch(vkCreateXcbSurfaceKHR(instanceVk, &surfaceCreateInfo, nullptr, &surfaceVk)){
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
	vkGetPhysicalDeviceQueueFamilyProperties(phyDeviceVk, &queueFamilyCount, nullptr);
	VkQueueFamilyProperties queueFamilies[queueFamilyCount];
	vkGetPhysicalDeviceQueueFamilyProperties(phyDeviceVk, &queueFamilyCount, queueFamilies);
	for(uint32_t i = 0; i < queueFamilyCount; ++i){
		// Graphics family
		if(queueFamilies[i].queueCount > 0 && queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			graphicFamily = i;
		}
		// Present family
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(phyDeviceVk, i, surfaceVk, &presentSupport);
		presentSupport &= vkGetPhysicalDeviceXcbPresentationSupportKHR(phyDeviceVk, i, connectionXcb, screenXcb->root_visual);
		if(queueFamilies[i].queueCount > 0 && presentSupport) {
			presentFamily = i;
		}
	}
	if(graphicFamily < 0 || presentFamily < 0){
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
	queueCreateInfos[0].queueFamilyIndex = graphicFamily;
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
	switch(vkCreateDevice(phyDeviceVk, &logicalDeviceCreateInfo, nullptr, &deviceVk)){
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
	vkGetDeviceQueue(deviceVk, graphicFamily, 0, &graphicQueueVk);
	vkGetDeviceQueue(deviceVk, presentFamily, 0, &presentQueueVk);
}

void DisplayXcb::handleXcbEvent(xcb_generic_event_t *event){
	if(event){
		switch(event->response_type & ~0x80){
			case XCB_CLIENT_MESSAGE:
				if(((xcb_client_message_event_t*)event)->data.data32[0] == closeReply->atom){
					server->stop();
				}
			break;
			default:
			break;
		}
	}
}