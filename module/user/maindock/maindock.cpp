#include "maindock.hpp"

MainDockModule::MainDockModule(User *user):user(user){
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
		throw "[MainDock image] Error create image";
	}
	// Image memory
	VkMemoryRequirements imageMemRequirements;
	vkGetImageMemoryRequirements(display->deviceVk, imageVk, &imageMemRequirements);
	VkMemoryAllocateInfo imageMemAllocInfo = {};
	imageMemAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	imageMemAllocInfo.allocationSize = imageMemRequirements.size;
	imageMemAllocInfo.memoryTypeIndex = display->findMemoryType(imageMemRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	if (vkAllocateMemory(display->deviceVk, &imageMemAllocInfo, nullptr, &imageMemoryVk) != VK_SUCCESS) {
		throw "[MainDock image] Error create image memory";
	}
	vkBindImageMemory(display->deviceVk, imageVk, imageMemoryVk, 0);
/*** Image view ***/
	VkImageViewCreateInfo imageViewInfo = {};
	imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewInfo.image = imageVk;
	imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewInfo.format = display->surfaceFormatVk.format;
	imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageViewInfo.subresourceRange.baseMipLevel = 0;
	imageViewInfo.subresourceRange.levelCount = 1;
	imageViewInfo.subresourceRange.baseArrayLayer = 0;
	imageViewInfo.subresourceRange.layerCount = 1;
	if (vkCreateImageView(display->deviceVk, &imageViewInfo, nullptr, &imageViewVk) != VK_SUCCESS) {
		throw "[MainDock image view] Error create image view";
	}
/*** Sampler ***/
	// Create
	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = 16;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;
	if (vkCreateSampler(display->deviceVk, &samplerInfo, nullptr, &samplerVk) != VK_SUCCESS) {
        throw "[SkyBox texture sampler] Error create sampler";
	}
	// Layout binding
	VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
	samplerLayoutBinding.binding = 0;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
/*** Descriptor pool & set ***/
	// Descriptor pool
	VkDescriptorPoolSize poolSize;
	poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSize.descriptorCount = 1;
	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = 1;
	if(vkCreateDescriptorPool(display->deviceVk, &poolInfo, nullptr, &descriptorPoolVk) != VK_SUCCESS) {
		throw "[Vulkan descriptor pool] Error create descriptor pool";
	}
	// Descriptor set layout
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &samplerLayoutBinding;
	switch(vkCreateDescriptorSetLayout(display->deviceVk, &layoutInfo, nullptr, &descriptorSetLayout)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan descriptor layout] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan descriptor layout] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		default:
		break;
	}
	// Descriptor set
	VkDescriptorSetAllocateInfo descriptorSetAllocInfo = {};
	descriptorSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocInfo.descriptorPool = descriptorPoolVk;
	descriptorSetAllocInfo.descriptorSetCount = 1;
	descriptorSetAllocInfo.pSetLayouts = &descriptorSetLayout;
	if(vkAllocateDescriptorSets(display->deviceVk, &descriptorSetAllocInfo, &descriptorSetVk) != VK_SUCCESS) {
		throw "[Vulkan descriptor set] Error create descriptor set";
	}
	VkDescriptorImageInfo descImageInfo = {};
	descImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	descImageInfo.imageView = imageViewVk;
	descImageInfo.sampler = samplerVk;
	// Descriptor write
	VkWriteDescriptorSet descriptorWrite;
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = descriptorSetVk;
	descriptorWrite.dstBinding = 0;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pImageInfo = &descImageInfo;
	vkUpdateDescriptorSets(display->deviceVk, 1, &descriptorWrite, 0, nullptr);
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
	vertexModuleVk = display->createShaderModule("vert/maindock.vert.spv");
	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertexModuleVk;
	vertShaderStageInfo.pName = "main";
	// fragment
	fragmentModuleVk = display->createShaderModule("frag/maindock.frag.spv");
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
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
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

void MainDockModule::recordCommand(Display *display){
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT | VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	beginInfo.pInheritanceInfo = &(display->inheritanceInfoVk);
	// Start command buffer
	vkBeginCommandBuffer(*commandBufferVk, &beginInfo);
	vkCmdBindDescriptorSets(*commandBufferVk, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayoutVk, 0, 1, &descriptorSetVk, 0, nullptr);
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
	paint();
}

void MainDockModule::paint(){
	SkCanvas *canvas = surfaceSkia->getCanvas();
	
	canvas->flush();
	user->display->paint();
}

void MainDockModule::removeModule(){
	delete this;
}

UserModule *createUserModule(void *user){
	return new MainDockModule((User *)user);
}