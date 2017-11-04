#include "startmenu.hpp"

StartMenuModule::StartMenuModule(User *user):user(user){
	Display *display = user->display;
/*** Image ***/
	// Create image
	VkImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.extent.width = display->displayExtentVk.width;
	imageCreateInfo.extent.height = display->displayExtentVk.height;
	imageCreateInfo.extent.depth = 1;
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.format = display->surfaceFormatVk.format;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.flags = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
	if (vkCreateImage(display->deviceVk, &imageCreateInfo, nullptr, &imageVk) != VK_SUCCESS) {
		throw "[Startmenu image] Error create image";
	}
	// Image memory
	VkMemoryRequirements imageMemRequirements;
	vkGetImageMemoryRequirements(display->deviceVk, imageVk, &imageMemRequirements);
	VkMemoryAllocateInfo imageMemAllocInfo = {};
	imageMemAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	imageMemAllocInfo.allocationSize = imageMemRequirements.size;
	imageMemAllocInfo.memoryTypeIndex = display->findMemoryType(imageMemRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	if (vkAllocateMemory(display->deviceVk, &imageMemAllocInfo, nullptr, &imageMemoryVk) != VK_SUCCESS) {
		throw "[Startmenu image] Error create image memory";
	}
	vkBindImageMemory(display->deviceVk, imageVk, imageMemoryVk, 0);
/*** Skia surface ***/
	// Context
	GrVkBackendContext* vkBackendCtxSkia = display->createSkiaContext();
	sk_sp<GrContext> contextSkia = GrContext::MakeVulkan(vkBackendCtxSkia);
	// Vulkan image info
	GrVkImageInfo imageInfoSkia = {};
	imageInfoSkia.fImage = imageVk;
	imageInfoSkia.fAlloc.fMemory = imageMemoryVk;
	imageInfoSkia.fAlloc.fOffset = 0;
	imageInfoSkia.fAlloc.fSize = imageMemAllocInfo.allocationSize;
	imageInfoSkia.fImageTiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfoSkia.fImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfoSkia.fFormat = display->surfaceFormatVk.format;
	imageInfoSkia.fLevelCount = 1;
	// Surface
	backendTextureSkia = GrBackendTexture(display->displayExtentVk.width, display->displayExtentVk.height, imageInfoSkia);
	surfaceSkia = SkSurface::MakeFromBackendTexture(
		contextSkia.get(), 
		backendTextureSkia, 
		kTopLeft_GrSurfaceOrigin, 
		1, 
		SkColorSpace::MakeSRGB(), 
		new SkSurfaceProps(SkSurfaceProps::InitType::kLegacyFontHost_InitType)
	);
	if (!surfaceSkia) {
        SkDebugf("SkSurface::MakeRenderTarget returned null\n");
    }
/*** Shader ***/
	// Vertex
	vertexModuleVk = display->createShaderModule("vert/startmenu.vert.spv");
	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertexModuleVk;
	vertShaderStageInfo.pName = "main";
	// Fragment
	fragmentModuleVk = display->createShaderModule("frag/startmenu.frag.spv");
	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragmentModuleVk;
	fragShaderStageInfo.pName = "main";
	// Shader stage
	VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};
/*** Fixed function ***/
	// Vertex input
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 0;
	vertexInputInfo.vertexAttributeDescriptionCount = 0;
	// Input assembly
	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;
	// Viewport& scissors
	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float) display->displayExtentVk.width;
	viewport.height = (float) display->displayExtentVk.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	VkRect2D scissor = {};
	scissor.offset = {0, 0};
	scissor.extent = display->displayExtentVk;
	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;
	// Rastertzer
	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	// Multisampling
	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	// Color blending
	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_TRUE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
/*** Pipeline ***/
	// Pipeline layout
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	switch(vkCreatePipelineLayout(display->deviceVk, &pipelineLayoutInfo, nullptr, &pipelineLayoutVk)) {
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan pipeline layout] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan pipeline layout] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		default:
		break;
	}
	// Create
	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = nullptr;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = nullptr;
	pipelineInfo.layout = pipelineLayoutVk;
	pipelineInfo.renderPass = display->renderPassVk;
	pipelineInfo.subpass = 0;
	switch(vkCreateGraphicsPipelines(display->deviceVk, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipelineVk)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan graphics pipeline] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan graphics pipeline] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		case VK_ERROR_INVALID_SHADER_NV:
			throw "[Vulkan graphics pipeline] VK_ERROR_INVALID_SHADER_NV";
		break;
		default:
		break;
	}
/*** Command buffer record ***/
	commandBufferVk = display->getSecCmdBuffer();
	recordCommand(display);
}

void StartMenuModule::recordCommand(Display *display){
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT | VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	beginInfo.pInheritanceInfo = &(display->inheritanceInfoVk);
	// Start command buffer
	vkBeginCommandBuffer(*commandBufferVk, &beginInfo);
	vkCmdBindPipeline(*commandBufferVk, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelineVk);
	vkCmdDraw(*commandBufferVk, 6, 1, 0, 0);
	// End command buffer
	switch(vkEndCommandBuffer(*commandBufferVk)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan command buffer record] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan command buffer record] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		default:
		break;
	}
	display->update();
	display->paint();
}

void StartMenuModule::removeModule(){
	delete this;
}

UserModule *createUserModule(void *user){
	return new StartMenuModule((User *)user);
}