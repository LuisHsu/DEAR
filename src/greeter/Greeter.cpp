#include <Greeter.hpp>

Greeter::Greeter():
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
	// Image create info
	VkImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
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
	imageCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
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
/*** Image Memory ***/
	// Get requirement
	VkMemoryRequirements memoryRequirement = {};
	vkGetImageMemoryRequirements(vkDevice, greeterClient->vkPresentImage, &memoryRequirement);
	// Allocate info
	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.allocationSize = memoryRequirement.size;
	memoryAllocateInfo.memoryTypeIndex = findMemoryType(memoryRequirement.memoryTypeBits, 0);
	// Allocate
	switch(vkAllocateMemory(vkDevice, &memoryAllocateInfo, nullptr, &(greeterClient->vkPresentImageMemory))){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan allocate image memory] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan allocate image memory] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_TOO_MANY_OBJECTS:
			throw "[Vulkan allocate image memory] VK_ERROR_TOO_MANY_OBJECTS";
		break;
		case VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR:
			throw "[Vulkan allocate image memory] VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR";
		break;
		default:
		break;
	}
	// Bind to image
	switch(vkBindImageMemory(vkDevice, greeterClient->vkPresentImage, greeterClient->vkPresentImageMemory, 0)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan bind image memory] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan bind image memory] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		default:
		break;
	}
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
/*** Staging Image ***/
	VkImageCreateInfo stagingImageCreateInfo = {};
	stagingImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	stagingImageCreateInfo.flags = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
	stagingImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	stagingImageCreateInfo.format = greeterClient->vkImageFormat;
	stagingImageCreateInfo.extent.width = greeterClient->vkImageExtent.width;
	stagingImageCreateInfo.extent.height = greeterClient->vkImageExtent.height;
	stagingImageCreateInfo.extent.depth = 1;
	stagingImageCreateInfo.mipLevels = 1;
	stagingImageCreateInfo.arrayLayers = 1;
	stagingImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	stagingImageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
	stagingImageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	stagingImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	stagingImageCreateInfo.queueFamilyIndexCount = 0;
	stagingImageCreateInfo.pQueueFamilyIndices = nullptr;
	stagingImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	switch(vkCreateImage(vkDevice, &stagingImageCreateInfo, nullptr, &(greeterClient->vkStagingImage))){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan create staging image] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan create staging image] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		default:
		break;
	}
/*** Staging memory ***/
	// Get requirement
	VkMemoryRequirements stagingMemoryRequirement = {};
	vkGetImageMemoryRequirements(vkDevice, greeterClient->vkStagingImage, &stagingMemoryRequirement);
	// Allocate info
	VkMemoryAllocateInfo stagingmemoryAllocateInfo = {};
	stagingmemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	stagingmemoryAllocateInfo.allocationSize = stagingMemoryRequirement.size;
	greeterClient->memSize = stagingMemoryRequirement.size;
	stagingmemoryAllocateInfo.memoryTypeIndex = findMemoryType(stagingMemoryRequirement.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	// Allocate
	switch(vkAllocateMemory(vkDevice, &stagingmemoryAllocateInfo, nullptr, &(greeterClient->vkStagingImageMemory))){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan allocate image memory] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan allocate image memory] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_TOO_MANY_OBJECTS:
			throw "[Vulkan allocate image memory] VK_ERROR_TOO_MANY_OBJECTS";
		break;
		case VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR:
			throw "[Vulkan allocate image memory] VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR";
		break;
		default:
		break;
	}
	// Bind to image
	switch(vkBindImageMemory(vkDevice, greeterClient->vkStagingImage, greeterClient->vkStagingImageMemory, 0)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan bind staging image memory] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan bind staging image memory] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		default:
		break;
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
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
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
	VkSubpassDependency dependencies = {};
	dependencies.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies.dstSubpass = 0;
	dependencies.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies.srcAccessMask = 0;
	dependencies.dstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
	dependencies.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
	// Render pass
	VkRenderPassCreateInfo renderPassCreateInfo = {};
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.attachmentCount = 1;
	renderPassCreateInfo.pAttachments = &colorAttachment;
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpass;
	renderPassCreateInfo.dependencyCount = 1;
	renderPassCreateInfo.pDependencies = &dependencies;
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
/*** Copy event ***/
	// Create
	VkEventCreateInfo imageCopyEventInfo = {};
	imageCopyEventInfo.sType = VK_STRUCTURE_TYPE_EVENT_CREATE_INFO;
	switch(vkCreateEvent(vkDevice, &imageCopyEventInfo, nullptr, &(greeterClient->vkImageCopyEvent))){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan create event] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan create event] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		default:
		break;
	}
	// Set event
	switch(vkSetEvent(vkDevice, greeterClient->vkImageCopyEvent)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan set event] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan set event] VK_ERROR_OUT_OF_DEVICE_MEMORY";
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
	// Image barrier
	VkImageSubresourceRange subresourceRange = {};
	subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.levelCount = 1;
	subresourceRange.baseArrayLayer = 0;
	subresourceRange.layerCount = 1;
	VkImageMemoryBarrier stagingImageMemoryBarrier = {};
	stagingImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	stagingImageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	stagingImageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	stagingImageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
	stagingImageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	stagingImageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	stagingImageMemoryBarrier.image = greeterClient->vkStagingImage;
	stagingImageMemoryBarrier.subresourceRange = subresourceRange;
	vkCmdPipelineBarrier(greeterClient->vkCommandBuffer,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_DEPENDENCY_BY_REGION_BIT,
		0, nullptr,
		0, nullptr,
		1, &stagingImageMemoryBarrier
	);
	// Wait for host copy complete
	vkCmdWaitEvents(greeterClient->vkCommandBuffer,
		1,
		&(greeterClient->vkImageCopyEvent),
		VK_PIPELINE_STAGE_HOST_BIT,
		VK_PIPELINE_STAGE_HOST_BIT,
		0, nullptr,
		0, nullptr,
		0, nullptr
	);
	// Copy to staging tmage
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
	imageCopyExtent.width = greeterClient->vkImageExtent.width;
	imageCopyExtent.height = greeterClient->vkImageExtent.height;
	imageCopyExtent.depth = 1;
	VkImageCopy imageCopy = {};
	imageCopy.srcSubresource = subResourceLayer;
	imageCopy.srcOffset = imageCopyOffset;
	imageCopy.dstSubresource = subResourceLayer;
	imageCopy.dstOffset = imageCopyOffset;
	imageCopy.extent = imageCopyExtent;
	vkCmdCopyImage(greeterClient->vkCommandBuffer,
		greeterClient->vkPresentImage,
		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		greeterClient->vkStagingImage,
		VK_IMAGE_LAYOUT_GENERAL,
		1,
		&imageCopy
	);
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
/*** Create fence ***/
	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	switch(vkCreateFence(vkDevice, &fenceCreateInfo, nullptr, &(greeterClient->vkStartTransferFence))){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan create fence] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan create fence] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		default:
		break;
	}
/*** Transfer thread ***/
	greeterClient->isPainting = true;
	greeterClient->transferThread = new std::thread(GreeterClient::texelTransfer, client, this);
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
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_TRANSFER_BIT};
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &(client->vkCommandBuffer);
	submitInfo.signalSemaphoreCount = 0;
	switch(vkQueueSubmit(vkGraphicsQueue, 1, &submitInfo, client->vkStartTransferFence)){
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
int32_t Greeter::findMemoryType(uint32_t memoryTypeBits, VkMemoryPropertyFlags properties){
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(vkPhyDevice, &memProperties);
	for(int32_t i = 0; i < 32; ++i){
		if ((memoryTypeBits & (1 << i)) && ((memProperties.memoryTypes[i].propertyFlags & properties) == properties)){
			return i;
		}
	}
	return -1;
}
void GreeterClient::texelTransfer(AreaClient *client, Greeter *greeter){
	GreeterClient *greeterClient = (GreeterClient *)client->data;
	while(greeterClient->isPainting){
		// Wait for transfer fence
		if(vkWaitForFences(greeter->vkDevice, 1, &(greeterClient->vkStartTransferFence), VK_TRUE, 3000000000) == VK_SUCCESS){
			// Reset fence
			switch(vkResetFences(greeter->vkDevice, 1, &(greeterClient->vkStartTransferFence))){
				case VK_ERROR_OUT_OF_HOST_MEMORY:
					throw "[Vulkan reset fence] VK_ERROR_OUT_OF_HOST_MEMORY";
				break;
				case VK_ERROR_OUT_OF_DEVICE_MEMORY:
					throw "[Vulkan reset fence] VK_ERROR_OUT_OF_DEVICE_MEMORY";
				break;
				default:
				break;
			}
			// Reset event
			switch(vkResetEvent(greeter->vkDevice, greeterClient->vkImageCopyEvent)){
				case VK_ERROR_OUT_OF_HOST_MEMORY:
					throw "[Vulkan reset event] VK_ERROR_OUT_OF_HOST_MEMORY";
				break;
				case VK_ERROR_OUT_OF_DEVICE_MEMORY:
					throw "[Vulkan reset event] VK_ERROR_OUT_OF_DEVICE_MEMORY";
				break;
				default:
				break;
			}
			// Map memory
			char *memPtr = nullptr;
			switch(vkMapMemory(greeter->vkDevice, greeterClient->vkStagingImageMemory, 0, greeterClient->memSize, 0, (void **)&memPtr)){
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
			// IPC message header
			IPCMessage frameMessage;
			frameMessage.type = IPC_Frame;
			frameMessage.length = sizeof(frameMessage) + greeterClient->memSize;
			// Copy memory
			std::vector<char> frameData(sizeof(frameMessage) + greeterClient->memSize);
			memcpy(frameData.data(), &frameMessage, sizeof(frameMessage));
			memcpy(frameData.data() + sizeof(frameMessage), memPtr, greeterClient->memSize);
			// Unmap memory
			vkUnmapMemory(greeter->vkDevice, greeterClient->vkStagingImageMemory);
			// Set event
			switch(vkResetEvent(greeter->vkDevice, greeterClient->vkImageCopyEvent)){
				case VK_ERROR_OUT_OF_HOST_MEMORY:
					throw "[Vulkan reset event] VK_ERROR_OUT_OF_HOST_MEMORY";
				break;
				case VK_ERROR_OUT_OF_DEVICE_MEMORY:
					throw "[Vulkan reset event] VK_ERROR_OUT_OF_DEVICE_MEMORY";
				break;
				default:
				break;
			}
			// Send message
			client->sendMessage((IPCMessage *)frameData.data());
		}
	}
}