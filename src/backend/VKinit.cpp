#include <Backend.hpp>

void Backend::initVK(BackendBase* &backend, BackendBase::LibType &lib, BackendBase::WMType &wm){
/*** Instance ***/
	// App info
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "DEVR";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
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
		throw "VK_KHR_surface is unsupported.";
	}else if(wm == BackendBase::WMType::WAYLAND_WM && !waylandSupport){
		throw "VK_KHR_wayland_surface is unsupported.";
	}else if(wm == BackendBase::WMType::XCB_WM && !xcbSupport){
		throw "VK_KHR_xcb_surface is unsupported.";
	}else if(wm == BackendBase::WMType::DISPLAY_WM && !displaySupport){
		throw "VK_KHR_display is unsupported.";
	}
	// Create backend
	if(waylandSupport){
		extensionNames[1] = "VK_KHR_wayland_surface";
		backend = new BackendWayland;
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
	}else if(xcbSupport){
		extensionNames[1] = "VK_KHR_xcb_surface";
		backend = new BackendXcb();
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
	}else if(displaySupport){
		extensionNames[1] = "VK_KHR_display";
		backend = new BackendDisplay();
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
		throw "No any protocol support and display is unsupported.";
	}
	
/*** Physical devices ***/
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(backend->vkInstance, &deviceCount, nullptr);
	backend->vkDevices.resize(deviceCount);
	VkPhysicalDevice devices[deviceCount];
	vkEnumeratePhysicalDevices(backend->vkInstance, &deviceCount, devices);
	for(uint32_t i = 0; i < deviceCount; ++i){
		backend->vkDevices.at(i).device = devices[i];
	}
}