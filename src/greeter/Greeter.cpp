#include <Greeter.hpp>

Greeter::Greeter():
	vkGraphicsFamily(-1)
{
	VkPhysicalDevice vkPhyDevice;
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
	instanceCreateInfo.enabledLayerCount = 1;
	const char *layerNames[1];
	layerNames[0] = "VK_LAYER_LUNARG_standard_validation";
	instanceCreateInfo.ppEnabledLayerNames = layerNames;
	// Extensions
	instanceCreateInfo.enabledExtensionCount = 3;
	const char *extensionNames[instanceCreateInfo.enabledExtensionCount];
	extensionNames[0] = "VK_KHR_surface";
	extensionNames[1] = "VK_EXT_debug_report";
	extensionNames[2] = "VK_KHR_external_memory_capabilities";
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
Greeter::~Greeter(){
	vkDestroyDevice(vkDevice, nullptr);
	vkDestroyInstance(vkInstance, nullptr);
}
void Greeter::initClient(IPCMessage *message, AreaClient *client){
	GreeterClient *greeterClient = (GreeterClient *)client->data;
	IPCConnectMessage *connectMessage = (IPCConnectMessage *)message;
	greeterClient->vkImageExtent = connectMessage->extent;
	greeterClient->vkImageFormat = connectMessage->format;
	VkShaderModule vkVertexShader;
	VkShaderModule vkFragmentShader;

/*** Image ***/
	// External image create info
	VkExternalMemoryImageCreateInfoKHR externalImageCreateInfo = {};
	externalImageCreateInfo.sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO_KHR;
	externalImageCreateInfo.handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT_KHR;
	// Image create info
	VkImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.pNext = &externalImageCreateInfo;
	imageCreateInfo.flags = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format = greeterClient->vkImageFormat;
	imageCreateInfo.extent.width = greeterClient->vkImageExtent.width;
	imageCreateInfo.extent.height = greeterClient->vkImageExtent.height;
	imageCreateInfo.extent.depth = 1;
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.queueFamilyIndexCount = 0;
	imageCreateInfo.pQueueFamilyIndices = nullptr;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	switch(vkCreateImage(vkDevice, &imageCreateInfo, nullptr, &(greeterClient->vkPresentImage))){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan create image] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan create image] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		default:
		break;
	}
/*** External Memory ***/
	// Get requirement
	VkMemoryRequirements memoryRequirement = {};
	vkGetImageMemoryRequirements(vkDevice, greeterClient->vkPresentImage, &memoryRequirement);
	// Allocate info
	VkExportMemoryAllocateInfoKHR exportAllocateInfo = {};
	exportAllocateInfo.sType = VK_STRUCTURE_TYPE_EXPORT_MEMORY_ALLOCATE_INFO_KHR;
	exportAllocateInfo.handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT_KHR;
	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.pNext = &exportAllocateInfo;
	memoryAllocateInfo.allocationSize = memoryRequirement.size;
	memoryAllocateInfo.memoryTypeIndex = ffs(memoryRequirement.memoryTypeBits) - 1;
	// Allocate
	switch(vkAllocateMemory(vkDevice, &memoryAllocateInfo, nullptr, &(greeterClient->vkExportMemory))){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan allocate export memory] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan allocate export memory] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_TOO_MANY_OBJECTS:
			throw "[Vulkan allocate export memory] VK_ERROR_TOO_MANY_OBJECTS";
		break;
		case VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR:
			throw "[Vulkan allocate export memory] VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR";
		break;
		default:
		break;
	}
	// Bind to image
	switch(vkBindImageMemory(vkDevice, greeterClient->vkPresentImage, greeterClient->vkExportMemory, 0)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan bind export memory] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan bind export memory] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		default:
		break;
	}
	// Create socket
	greeterClient->displayFd = socket(AF_UNIX, SOCK_STREAM, 0);
	// Fill address
	greeterClient->displayDMAddr.sun_family = AF_UNIX;
	strcpy(greeterClient->displayDMAddr.sun_path, connectMessage->displayFile);
	greeterClient->displayAddr.sun_family = AF_UNIX;
	sprintf(greeterClient->displayAddr.sun_path, "%s-greeter", connectMessage->displayFile);
	// Bind
	if(bind(greeterClient->displayFd, (struct sockaddr *)&(greeterClient->displayAddr), sizeof(greeterClient->displayAddr)) < 0){
		close(greeterClient->displayFd);
		throw "[Greeter display socket] Can't bind socket.";
	}
	// Connect
	if(connect(greeterClient->displayFd, (struct sockaddr *)&(greeterClient->displayDMAddr), sizeof(greeterClient->displayDMAddr)) < 0){
		std::cerr << strerror(errno) << std::endl;
		close(greeterClient->displayFd);
		throw "[Greeter display socket] Can't connect to DM display.";
	}
	// Get memory fd
	VkMemoryGetFdInfoKHR getFdInfo = {};
	getFdInfo.sType = VK_STRUCTURE_TYPE_MEMORY_GET_FD_INFO_KHR;
	getFdInfo.memory = greeterClient->vkExportMemory;
	getFdInfo.handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT_KHR;
	switch(vkGetMemoryFdKHR(vkDevice, &getFdInfo, &(greeterClient->memoryFd))){
		case VK_ERROR_TOO_MANY_OBJECTS:
			throw "[Greeter get memory file descriptor] VK_ERROR_TOO_MANY_OBJECTS";
		break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Greeter get memory file descriptor] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_EXTENSION_NOT_PRESENT:
			throw "[Greeter get memory file descriptor] VK_ERROR_EXTENSION_NOT_PRESENT";
		break;
		default:
		break;
	}
	// Send memory fd
	sendDisplayFd(greeterClient->displayFd, greeterClient->memoryFd);

/*** Image View ***/
	VkImageViewCreateInfo imageViewCreateInfo = {};
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.image = greeterClient->vkPresentImage;
	imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewCreateInfo.format = greeterClient->vkImageFormat;
	imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
	imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
	imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
	imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
	imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;
	if(vkCreateImageView(vkDevice, &imageViewCreateInfo, nullptr, &(greeterClient->vkPresentImageView))!= VK_SUCCESS){
		throw "[Vulkan image view] Error create image view.";
	}


/*** Shader ***/
	// Vertex shader
	vkVertexShader = createShaderModule("vert/Greeter.vert.spv");
	VkPipelineShaderStageCreateInfo vertexShaderStageCreateInfo = {};
	vertexShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertexShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertexShaderStageCreateInfo.module = vkVertexShader;
	vertexShaderStageCreateInfo.pName = "main";
	vkFragmentShader = createShaderModule("frag/Greeter.frag.spv");
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
	viewport.width = (float) greeterClient->vkImageExtent.width;
	viewport.height = (float) greeterClient->vkImageExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	// Scissor
	VkRect2D scissor = {};
	scissor.offset = {0, 0};
	scissor.extent = greeterClient->vkImageExtent;
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
	// Pipeline layout
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pSetLayouts = nullptr;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = 0;
	switch(vkCreatePipelineLayout(vkDevice, &pipelineLayoutInfo, nullptr, &(greeterClient->vkPipelineLayout))){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan pipeline layout] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan pipeline layout] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		default:
		break;
	}
/*** Render pass ***/
	// Color attachment
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = greeterClient->vkImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
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
	switch(vkCreateRenderPass(vkDevice, &renderPassCreateInfo, nullptr, &(greeterClient->vkRenderPass))){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan render pass] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
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
	pipelineInfo.renderPass = greeterClient->vkRenderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -1;
	pipelineInfo.layout = greeterClient->vkPipelineLayout;
	switch(vkCreateGraphicsPipelines(vkDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &(greeterClient->vkGraphicsPipeline))){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan pipeline] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan pipeline] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		case VK_ERROR_INVALID_SHADER_NV:
			throw "[Vulkan pipeline] VK_ERROR_INVALID_SHADER_NV";
		break;
		default:
		break;
	}
	vkDestroyShaderModule(vkDevice, vkVertexShader, nullptr);
	vkDestroyShaderModule(vkDevice, vkFragmentShader, nullptr);
/*** Frame buffer ***/
	VkFramebufferCreateInfo framebufferInfo = {};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = greeterClient->vkRenderPass;
	framebufferInfo.attachmentCount = 1;
	framebufferInfo.pAttachments = &(greeterClient->vkPresentImageView);
	framebufferInfo.width = greeterClient->vkImageExtent.width;
	framebufferInfo.height = greeterClient->vkImageExtent.height;
	framebufferInfo.layers = 1;
	switch(vkCreateFramebuffer(vkDevice, &framebufferInfo, nullptr, &(greeterClient->vkFramebuffer))){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan framebuffer] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan framebuffer] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		default:
		break;
	}
/*** Command pool ***/
	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = vkGraphicsFamily;
	poolInfo.flags = 0;
	switch(vkCreateCommandPool(vkDevice, &poolInfo, nullptr, &(greeterClient->vkCommandPool))){
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
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = greeterClient->vkCommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;
	switch(vkAllocateCommandBuffers(vkDevice, &allocInfo, &(greeterClient->vkCommandBuffer))){
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
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	beginInfo.pInheritanceInfo = nullptr;
	// Start command buffer
	vkBeginCommandBuffer(greeterClient->vkCommandBuffer, &beginInfo);
	// Start render pass
	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = greeterClient->vkRenderPass;
	renderPassInfo.framebuffer = greeterClient->vkFramebuffer;
	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent = greeterClient->vkImageExtent;
	VkClearValue clearColor = {0.0f, 1.0f, 0.0f, 1.0f};
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;
	vkCmdBeginRenderPass(greeterClient->vkCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(greeterClient->vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, greeterClient->vkGraphicsPipeline);
	vkCmdDraw(greeterClient->vkCommandBuffer, 3, 1, 0, 0);
	vkCmdEndRenderPass(greeterClient->vkCommandBuffer);
	switch(vkEndCommandBuffer(greeterClient->vkCommandBuffer)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan command buffer record] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan command buffer record] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		default:
		break;
	}
/*** Create semaphore ***/
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	switch(vkCreateSemaphore(vkDevice, &semaphoreInfo, nullptr, &(greeterClient->vkImageAvailableSemaphore))){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan image available semaphore] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan image available semaphore] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		default:
		break;
	}
}
void Greeter::handleMessage(IPCMessage *message, AreaClient *client){
	switch(message->type){
		case IPC_Connect:{
			GreeterClient *greeterClient = new GreeterClient;
			client->data = greeterClient;
			initClient(message, client);
			paint(greeterClient);
		}break;
		default:
		break;
	}
}

VkResult Greeter::vkGetMemoryFdKHR(VkDevice device,  const VkMemoryGetFdInfoKHR *pGetFdInfo, int* pFd){
	auto func = (PFN_vkGetMemoryFdKHR) vkGetInstanceProcAddr(vkInstance, "vkGetMemoryFdKHR");
	if (func != nullptr) {
		 VkResult res = func(device, pGetFdInfo, pFd);
		 return res;
	}else{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

VkShaderModule Greeter::createShaderModule(const char *fileName){
	// Read code
	std::ifstream fin(fileName, std::ios::ate | std::ios::binary);
	if (!fin.is_open()) {
		throw "[Vulkan shader] failed to open file";
	}
	uint32_t codeSize = (size_t)fin.tellg();
	std::vector<char> shaderCode(codeSize);
	fin.seekg(0);
	fin.read(shaderCode.data(), codeSize);
	fin.close();
	// Create
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = codeSize;
	createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());
	VkShaderModule ret;
	switch(vkCreateShaderModule(vkDevice, &createInfo, nullptr, &ret)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan shader] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan shader] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		case VK_ERROR_INVALID_SHADER_NV:
			throw "[Vulkan shader] VK_ERROR_INVALID_SHADER_NV";
		break;
		default:
		break;
	}
	return ret;
}

void Greeter::paint(GreeterClient *client){
	// Submit command buffer
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 0;
	//submitInfo.pWaitSemaphores = &(client->vkImageAvailableSemaphore);
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &(client->vkCommandBuffer);
	submitInfo.signalSemaphoreCount = 0;
	//submitInfo.pSignalSemaphores = &(client->vkImageAvailableSemaphore);
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
}

void Greeter::sendDisplayFd(int sockfd, int memfd){
	struct iovec iov;
    struct msghdr msg;
	char buf[2];
	struct cmsghdr *cmptr;

    iov.iov_base = buf;
    iov.iov_len  = 2;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    if (memfd < 0) {
        msg.msg_control = NULL;
        msg.msg_controllen = 0;
        buf[1] = -memfd;
        if (buf[1] == 0)
            buf[1] = 1;
    } else {
        if ((cmptr = (struct cmsghdr *)malloc(CMSG_LEN(sizeof(int)))) == nullptr){
			throw "[Greeter send memory file descriptor] Can't create control message header.";
		}
        cmptr->cmsg_level  = SOL_SOCKET;
        cmptr->cmsg_type   = SCM_RIGHTS;
        cmptr->cmsg_len    = CMSG_LEN(sizeof(int));
        msg.msg_control    = cmptr;
        msg.msg_controllen = CMSG_LEN(sizeof(int));
        *(int *)CMSG_DATA(cmptr) = memfd;
        buf[1] = 0;
    }
    buf[0] = 0;
    if(sendmsg(sockfd, &msg, 0) != 2){
		throw "[Greeter send memory file descriptor] Error sending file descriptor.";
	}
}