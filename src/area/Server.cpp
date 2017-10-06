#include <AreaSocket.hpp>

AreaServer::AreaServer(const char *path){
	// Create socket
	listenFd = socket(AF_UNIX, SOCK_STREAM, 0);
	// Fill address
	listenAddr.sun_family = AF_UNIX;
	strcpy(listenAddr.sun_path, path);
	// Unlink
	unlink(listenAddr.sun_path);
	// Bind
	if(bind(listenFd, (struct sockaddr *)&listenAddr, sizeof(listenAddr)) < 0){
		close(listenFd);
		throw "[Area server] Can't bind socket.";
	}
}

AreaServer::~AreaServer(){
	if(listenFd >= 0){
		close(listenFd);
	}
}

void AreaServer::start(AreaServerHandler *handler){
	this->handler = handler;
	// Set event base
	evutil_make_socket_nonblocking(listenFd);
	eventBase = event_base_new();
	if(eventBase == nullptr){
		std::cerr << strerror(errno) << std::endl;
		close(listenFd);
		throw "[Area server] Can't create event base.";
	}
	acceptEvent = event_new(eventBase, listenFd, EV_READ|EV_PERSIST, socketAccept, this);
	event_add(acceptEvent, nullptr);
	// Listen
	if(listen(listenFd, 10) < 0){
		close(listenFd);
		throw "[Area server] Can't listen server.";
	}
	event_base_dispatch(eventBase);
}

void AreaServer::socketAccept(evutil_socket_t listenFd, short event, void *arg){
	AreaServer *server = (AreaServer *)arg;
	// Accept
	struct sockaddr_un clientAddr;
	uint32_t clientAddrLen;
	int clientFd = accept(listenFd, (struct sockaddr *)&clientAddr, &clientAddrLen);
	if(clientFd >= 0){
		AreaClient *newClient = new AreaClient(
			server->clients.size(),
			clientFd,
			clientAddr,
			clientAddrLen,
			server,
			server->eventBase,
			server->handler
		);
		server->clients.push_back(newClient);
	}
}

void AreaServer::eraseClient(uint32_t index){
	std::list<AreaClient *>::iterator it = clients.begin();
	advance(it, index);
	delete *it;
	clients.erase(it);
}

ssize_t AreaClient::sendMessage(IPCMessage *message){
	return send(clientFd, message, message->length, 0);
}

AreaServerHandler::AreaServerHandler():
	vkGraphicsFamily(-1)
{
/*** Instance ***/
	// App info
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Dear greeter";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;
	// Create info
	VkInstanceCreateInfo instanceCreateInfo = {};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pApplicationInfo = &appInfo;
	#ifdef NDEBUG
		instanceCreateInfo.enabledLayerCount = 0;
		// Extensions
		instanceCreateInfo.enabledExtensionCount = 1;
		const char *extensionNames[instanceCreateInfo.enabledExtensionCount];
		extensionNames[0] = "VK_KHR_surface";
		instanceCreateInfo.ppEnabledExtensionNames = extensionNames;
	#else
		instanceCreateInfo.enabledLayerCount = 1;
		const char *layerNames[1];
		layerNames[0] = "VK_LAYER_LUNARG_standard_validation";
		instanceCreateInfo.ppEnabledLayerNames = layerNames;
		// Extensions
		instanceCreateInfo.enabledExtensionCount = 2;
		const char *extensionNames[instanceCreateInfo.enabledExtensionCount];
		extensionNames[0] = "VK_KHR_surface";
		extensionNames[1] = "VK_EXT_debug_report";
		instanceCreateInfo.ppEnabledExtensionNames = extensionNames;
	#endif
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
		throw "[Vulkan physical device] No suitable device.";
	}

/*** Queue family ***/
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(vkPhyDevice, &queueFamilyCount, nullptr);
	VkQueueFamilyProperties queueFamilies[queueFamilyCount];
	vkGetPhysicalDeviceQueueFamilyProperties(vkPhyDevice, &queueFamilyCount, queueFamilies);
	for(uint32_t i = 0; i < queueFamilyCount; ++i){
		// Graphics family
		if(queueFamilies[i].queueCount > 0 && queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			vkGraphicsFamily = i;
		}
	}
	if(vkGraphicsFamily < 0){
		throw "[Vulkan queue family] No suitable queue families.";
	}
/*** Logical device ***/
	// Graphics queue info
	VkDeviceQueueCreateInfo queueCreateInfo;
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = vkGraphicsFamily;
	queueCreateInfo.queueCount = 1;
	queueCreateInfo.flags = 0;
	float queuePriority = 1.0f;
	queueCreateInfo.pQueuePriorities = &queuePriority;
	// Create logical device
	VkPhysicalDeviceFeatures deviceFeatures = {};
	VkDeviceCreateInfo logicalDeviceCreateInfo = {};
	logicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	logicalDeviceCreateInfo.queueCreateInfoCount = 1;
	logicalDeviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
	logicalDeviceCreateInfo.pEnabledFeatures = &deviceFeatures;
	logicalDeviceCreateInfo.enabledExtensionCount = 4;
	const char *devExtensionNames[logicalDeviceCreateInfo.enabledExtensionCount];
	devExtensionNames[0] = "VK_KHR_external_memory";
	devExtensionNames[1] = "VK_KHR_external_memory_fd";
	devExtensionNames[2] = "VK_KHR_external_semaphore";
	devExtensionNames[3] = "VK_KHR_external_semaphore_fd";
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
	vkGetDeviceQueue(vkDevice, vkGraphicsFamily, 0, &vkGraphicsQueue);
}