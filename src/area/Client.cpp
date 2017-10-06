#include <AreaSocket.hpp>

void AreaClient::initClient(IPCMessage *message){
	IPCConnectMessage *connectMessage = (IPCConnectMessage *)message;
	vkImageExtent = connectMessage->extent;
	vkImageFormat = connectMessage->format;
	VkShaderModule vkVertexShader;
	VkShaderModule vkFragmentShader;

/*** Image ***/
	// Image create info
	VkImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.flags = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format = vkImageFormat;
	imageCreateInfo.extent.width = vkImageExtent.width;
	imageCreateInfo.extent.height = vkImageExtent.height;
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
	switch(vkCreateImage(area->vkDevice, &imageCreateInfo, nullptr, &vkPresentImage)){
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
	vkGetImageMemoryRequirements(area->vkDevice, vkPresentImage, &memoryRequirement);
	// Allocate info
	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.allocationSize = memoryRequirement.size;
	memoryAllocateInfo.memoryTypeIndex = findMemoryType(area->vkPhyDevice, memoryRequirement.memoryTypeBits, 0);
	// Allocate
	switch(vkAllocateMemory(area->vkDevice, &memoryAllocateInfo, nullptr, &vkPresentImageMemory)){
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
	switch(vkBindImageMemory(area->vkDevice, vkPresentImage, vkPresentImageMemory, 0)){
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
	imageViewCreateInfo.image = vkPresentImage;
	imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewCreateInfo.format = vkImageFormat;
	imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
	imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
	imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
	imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
	imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;
	if(vkCreateImageView(area->vkDevice, &imageViewCreateInfo, nullptr, &vkPresentImageView)!= VK_SUCCESS){
		throw "[Vulkan image view] Error create image view.";
	}
/*** Staging Image ***/
	VkImageCreateInfo stagingImageCreateInfo = {};
	stagingImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	stagingImageCreateInfo.flags = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
	stagingImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	stagingImageCreateInfo.format = vkImageFormat;
	stagingImageCreateInfo.extent.width = vkImageExtent.width;
	stagingImageCreateInfo.extent.height = vkImageExtent.height;
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
	switch(vkCreateImage(area->vkDevice, &stagingImageCreateInfo, nullptr, &vkStagingImage)){
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
	vkGetImageMemoryRequirements(area->vkDevice, vkStagingImage, &stagingMemoryRequirement);
	// Allocate info
	VkMemoryAllocateInfo stagingmemoryAllocateInfo = {};
	stagingmemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	stagingmemoryAllocateInfo.allocationSize = stagingMemoryRequirement.size;
	memSize = stagingMemoryRequirement.size;
	stagingmemoryAllocateInfo.memoryTypeIndex = findMemoryType(area->vkPhyDevice, stagingMemoryRequirement.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	// Allocate
	switch(vkAllocateMemory(area->vkDevice, &stagingmemoryAllocateInfo, nullptr, &vkStagingImageMemory)){
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
	switch(vkBindImageMemory(area->vkDevice, vkStagingImage, vkStagingImageMemory, 0)){
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
	viewport.width = (float) vkImageExtent.width;
	viewport.height = (float) vkImageExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	// Scissor
	VkRect2D scissor = {};
	scissor.offset = {0, 0};
	scissor.extent = vkImageExtent;
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
	switch(vkCreatePipelineLayout(area->vkDevice, &pipelineLayoutInfo, nullptr, &vkPipelineLayout)){
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
	colorAttachment.format = vkImageFormat;
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
	switch(vkCreateRenderPass(area->vkDevice, &renderPassCreateInfo, nullptr, &vkRenderPass)){
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
	pipelineInfo.renderPass = vkRenderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -1;
	pipelineInfo.layout = vkPipelineLayout;
	switch(vkCreateGraphicsPipelines(area->vkDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &vkGraphicsPipeline)){
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
	vkDestroyShaderModule(area->vkDevice, vkVertexShader, nullptr);
	vkDestroyShaderModule(area->vkDevice, vkFragmentShader, nullptr);
/*** Frame buffer ***/
	VkFramebufferCreateInfo framebufferInfo = {};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = vkRenderPass;
	framebufferInfo.attachmentCount = 1;
	framebufferInfo.pAttachments = &vkPresentImageView;
	framebufferInfo.width = vkImageExtent.width;
	framebufferInfo.height = vkImageExtent.height;
	framebufferInfo.layers = 1;
	switch(vkCreateFramebuffer(area->vkDevice, &framebufferInfo, nullptr, &vkFramebuffer)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan framebuffer] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan framebuffer] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		default:
		break;
	}
/*** Event ***/
	// Create
	VkEventCreateInfo imageCopyEventInfo = {};
	imageCopyEventInfo.sType = VK_STRUCTURE_TYPE_EVENT_CREATE_INFO;
	switch(vkCreateEvent(area->vkDevice, &imageCopyEventInfo, nullptr, &vkImageCopyEvent)){
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
	switch(vkSetEvent(area->vkDevice, vkImageCopyEvent)){
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
	poolInfo.queueFamilyIndex = area->vkGraphicsFamily;
	poolInfo.flags = 0;
	switch(vkCreateCommandPool(area->vkDevice, &poolInfo, nullptr, &vkCommandPool)){
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
	allocInfo.commandPool = vkCommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;
	switch(vkAllocateCommandBuffers(area->vkDevice, &allocInfo, &vkCommandBuffer)){
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
	vkBeginCommandBuffer(vkCommandBuffer, &beginInfo);
	// Start render pass
	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = vkRenderPass;
	renderPassInfo.framebuffer = vkFramebuffer;
	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent = vkImageExtent;
	VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;
	vkCmdBeginRenderPass(vkCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkGraphicsPipeline);
	vkCmdDraw(vkCommandBuffer, 3, 1, 0, 0);
	vkCmdEndRenderPass(vkCommandBuffer);
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
	stagingImageMemoryBarrier.image = vkStagingImage;
	stagingImageMemoryBarrier.subresourceRange = subresourceRange;
	vkCmdPipelineBarrier(vkCommandBuffer,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_DEPENDENCY_BY_REGION_BIT,
		0, nullptr,
		0, nullptr,
		1, &stagingImageMemoryBarrier
	);
	// Wait for host copy complete
	vkCmdWaitEvents(vkCommandBuffer,
		1,
		&vkImageCopyEvent,
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
	imageCopyExtent.width = vkImageExtent.width;
	imageCopyExtent.height = vkImageExtent.height;
	imageCopyExtent.depth = 1;
	VkImageCopy imageCopy = {};
	imageCopy.srcSubresource = subResourceLayer;
	imageCopy.srcOffset = imageCopyOffset;
	imageCopy.dstSubresource = subResourceLayer;
	imageCopy.dstOffset = imageCopyOffset;
	imageCopy.extent = imageCopyExtent;
	vkCmdCopyImage(vkCommandBuffer,
		vkPresentImage,
		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		vkStagingImage,
		VK_IMAGE_LAYOUT_GENERAL,
		1,
		&imageCopy
	);
	switch(vkEndCommandBuffer(vkCommandBuffer)){
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
	switch(vkCreateFence(area->vkDevice, &fenceCreateInfo, nullptr, &vkStartTransferFence)){
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
	// Semaphore
	sem_init(&transferFinishedSem, 0, 1);
	// Thread
	isPainting = true;
	transferThread = new std::thread(texelTransfer, this, area);
}
AreaClient::AreaClient(
	uint32_t index,
	int fd,
	struct sockaddr_un addr,
	uint32_t addrlen,
	AreaServer *server,
	struct event_base *eventBase,
	AreaServerHandler *area
):
	index(index), clientFd(fd), addr(addr), addrlen(addrlen), server(server), area(area)
{
	// Add new event
	receiveEvent = event_new(eventBase, clientFd, EV_READ|EV_PERSIST, socketReceive, this);
	event_add(receiveEvent, nullptr);
	// Send connect message
	IPCMessage connectMessage;
	connectMessage.type = IPC_Connect;
	connectMessage.length = sizeof(connectMessage);
	if(send(clientFd, &connectMessage, connectMessage.length, 0) < 0){
		close(clientFd);
		throw "[Area client] Can't send connect message.";
	}
}
int32_t AreaClient::findMemoryType(VkPhysicalDevice phyDevice, uint32_t memoryTypeBits, VkMemoryPropertyFlags properties){
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(phyDevice, &memProperties);
	for(int32_t i = 0; i < 32; ++i){
		if ((memoryTypeBits & (1 << i)) && ((memProperties.memoryTypes[i].propertyFlags & properties) == properties)){
			return i;
		}
	}
	return -1;
}
void AreaClient::socketReceive(evutil_socket_t fd, short event, void *arg){
	AreaClient *client = (AreaClient *)arg;
	IPCMessage header;
	std::vector<char> buffer;
	if(recv(fd, &header, sizeof(header), 0) > 0){
		buffer.resize(header.length);
		IPCMessage *message = (IPCMessage *)buffer.data();
		*message = header;
		int32_t received = sizeof(header);
		while(received < header.length){
			int curRecv = recv(fd, buffer.data() + received, header.length - received, 0);
			if(curRecv <= 0){
				std::cerr << "[Area client] Abort receiving from backend." << std::endl;
				break;
			}
			received += curRecv;
		}
		if(received == header.length){
			client->area->handleMessage(message, client);
		}
	}else{
		event_del(client->receiveEvent);
		event_free(client->receiveEvent);
		close(client->clientFd);
		client->server->eraseClient(client->index);
	}
}
void AreaClient::texelTransfer(AreaClient *client, AreaServerHandler *area){
	while(client->isPainting){
		// Wait for transfer fence
		if(vkWaitForFences(area->vkDevice, 1, &(client->vkStartTransferFence), VK_TRUE, 3000000000) == VK_SUCCESS){
			// Reset fence
			switch(vkResetFences(area->vkDevice, 1, &(client->vkStartTransferFence))){
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
			switch(vkResetEvent(area->vkDevice, client->vkImageCopyEvent)){
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
			switch(vkMapMemory(area->vkDevice, client->vkStagingImageMemory, 0, client->memSize, 0, (void **)&memPtr)){
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
			frameMessage.length = sizeof(frameMessage) + client->memSize;
			// Copy memory
			std::vector<char> frameData(sizeof(frameMessage) + client->memSize);
			memcpy(frameData.data(), &frameMessage, sizeof(frameMessage));
			memcpy(frameData.data() + sizeof(frameMessage), memPtr, client->memSize);
			// Unmap memory
			vkUnmapMemory(area->vkDevice, client->vkStagingImageMemory);
			// Set event
			switch(vkResetEvent(area->vkDevice, client->vkImageCopyEvent)){
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
			// Signal semaphore
			sem_post(&(client->transferFinishedSem));
		}
	}
}
VkShaderModule AreaClient::createShaderModule(const char *fileName){
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
	switch(vkCreateShaderModule(area->vkDevice, &createInfo, nullptr, &ret)){
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