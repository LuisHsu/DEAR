#include "skybox.hpp"

#include <iostream>

SkyBoxModule::SkyBoxModule(Area *area):area(area){
}

void SkyBoxModule::userInit(void *userPtr){
	User *user = (User *)userPtr;
	Display *display = user->display;
	VkCommandBuffer *cmdBuffer = display->getSecCmdBuffer();
	commandBuffers[user] = cmdBuffer;
/*** Shader ***/
	// Vertex
	vertexModuleVk = createShaderModule("vert/skybox.vert.spv", display->deviceVk);
	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertexModuleVk;
	vertShaderStageInfo.pName = "main";
	// Fragment
	fragmentModuleVk = createShaderModule("frag/skybox.frag.spv", display->deviceVk);
	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragmentModuleVk;
	fragShaderStageInfo.pName = "main";
	// Shader stage
	VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};
/*** Texture PNG ***/
	// Open
	FILE *textureIn = fopen("res/skybox.png", "rb");
	if(!textureIn){
		throw "[Skybox] Can't open image file";
	}
	// Check if png
	{
		unsigned char header[8];
		fread(header, 1, 8, textureIn);
		if(png_sig_cmp(header, 0, 8)){
			throw "[Skybox] Image isn't a PNG file";
		}
		fseek(textureIn, 0, SEEK_SET);
	}
	// Init png struct
	png_structp ptrPng = png_create_read_struct(PNG_LIBPNG_VER_STRING, this, [](png_structp pngStr, png_const_charp msg){
		// Error function
		throw "[Skybox] Error reading PNG file";
	},nullptr);
	if (!ptrPng){
		throw "[Skybox] Error create PNG pointer";
	}
	png_infop infoPng = png_create_info_struct(ptrPng);
	if (!infoPng){
		png_destroy_read_struct(&ptrPng, (png_infopp)NULL, (png_infopp)NULL);
		throw "[Skybox] Error create PNG info pointer";
	}
	// Init io
	png_init_io(ptrPng, textureIn);
	// Read info
	png_read_info(ptrPng, infoPng);
	uint32_t textureHeight = png_get_image_height(ptrPng, infoPng);
	uint32_t textureWidth = png_get_image_width(ptrPng, infoPng);
	uint32_t textureRowBytes = png_get_rowbytes(ptrPng, infoPng);
/*** Texture buffer ***/
	// Buffer
	VkBuffer textureBuffer;
	VkBufferCreateInfo textureBufferInfo = {};
	textureBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	textureBufferInfo.size = textureHeight * textureRowBytes;
	textureBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	textureBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	switch(vkCreateBuffer(display->deviceVk, &textureBufferInfo, nullptr, &textureBuffer)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Skybox buffer create] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Skybox buffer create] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		default:
		break;
	}
	// Buffer memory
	VkDeviceMemory textureBufferMemory;
	VkMemoryRequirements textureMemRequirements;
	vkGetBufferMemoryRequirements(display->deviceVk, textureBuffer, &textureMemRequirements);
	VkMemoryAllocateInfo textureMemAllocInfo = {};
	textureMemAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	textureMemAllocInfo.allocationSize = textureMemRequirements.size;
	textureMemAllocInfo.memoryTypeIndex = display->findMemoryType(textureMemRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	if(vkAllocateMemory(display->deviceVk, &textureMemAllocInfo, nullptr, &textureBufferMemory) != VK_SUCCESS){
		throw "[Vulkan texture buffer] Error allocate memory";
	}
	vkBindBufferMemory(display->deviceVk, textureBuffer, textureBufferMemory, 0);
	// Load pixel
	void* textureMemoryData = nullptr;
	vkMapMemory(display->deviceVk, textureBufferMemory, 0, textureBufferInfo.size, 0, &textureMemoryData);
	png_bytep textureRowPtrs[textureHeight];
	for(uint32_t i = 0; i < textureHeight; ++i){
		textureRowPtrs[i] = (png_bytep)((char *)textureMemoryData + (i * textureRowBytes));
	}
	png_read_image(ptrPng, textureRowPtrs);
	vkUnmapMemory(display->deviceVk, textureBufferMemory);
	png_read_end(ptrPng, nullptr);
/*** Texture image ***/
	// Create image
	VkImageCreateInfo textureImageInfo = {};
	textureImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	textureImageInfo.imageType = VK_IMAGE_TYPE_2D;
	textureImageInfo.extent.width = textureWidth;
	textureImageInfo.extent.height = textureHeight;
	textureImageInfo.extent.depth = 1;
	textureImageInfo.mipLevels = 1;
	textureImageInfo.arrayLayers = 1;
	textureImageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	textureImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	textureImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	textureImageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	textureImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	textureImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	if (vkCreateImage(display->deviceVk, &textureImageInfo, nullptr, &textureImageVk) != VK_SUCCESS) {
		throw "[SkyBox texture image] Error create image";
	}
	// Image memory
	VkMemoryRequirements textureImageMemRequirements;
	vkGetImageMemoryRequirements(display->deviceVk, textureImageVk, &textureImageMemRequirements);
	VkMemoryAllocateInfo textureImageMemAllocInfo = {};
	textureImageMemAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	textureImageMemAllocInfo.allocationSize = textureImageMemRequirements.size;
	textureImageMemAllocInfo.memoryTypeIndex = display->findMemoryType(textureImageMemRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	if (vkAllocateMemory(display->deviceVk, &textureImageMemAllocInfo, nullptr, &textureImageMemoryVk) != VK_SUCCESS) {
		throw "[SkyBox texture image] Error create image memory";
	}
	vkBindImageMemory(display->deviceVk, textureImageVk, textureImageMemoryVk, 0);
/*** Texture image view ***/
	VkImageViewCreateInfo textureImageViewInfo = {};
	textureImageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	textureImageViewInfo.image = textureImageVk;
	textureImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	textureImageViewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	textureImageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	textureImageViewInfo.subresourceRange.baseMipLevel = 0;
	textureImageViewInfo.subresourceRange.levelCount = 1;
	textureImageViewInfo.subresourceRange.baseArrayLayer = 0;
	textureImageViewInfo.subresourceRange.layerCount = 1;
    if (vkCreateImageView(display->deviceVk, &textureImageViewInfo, nullptr, &textureImageViewVk) != VK_SUCCESS) {
        throw "[SkyBox texture image view] Error create image view";
	}
/*** Texture Sampler ***/
	// Create
	VkSamplerCreateInfo textureSamplerInfo = {};
	textureSamplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	textureSamplerInfo.magFilter = VK_FILTER_LINEAR;
	textureSamplerInfo.minFilter = VK_FILTER_LINEAR;
	textureSamplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	textureSamplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	textureSamplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	textureSamplerInfo.anisotropyEnable = VK_TRUE;
	textureSamplerInfo.maxAnisotropy = 16;
	textureSamplerInfo.unnormalizedCoordinates = VK_FALSE;
	textureSamplerInfo.compareEnable = VK_FALSE;
	textureSamplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	textureSamplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	textureSamplerInfo.mipLodBias = 0.0f;
	textureSamplerInfo.minLod = 0.0f;
	textureSamplerInfo.maxLod = 0.0f;
	if (vkCreateSampler(display->deviceVk, &textureSamplerInfo, nullptr, &textureSamplerVk) != VK_SUCCESS) {
        throw "[SkyBox texture sampler] Error create sampler";
	}
	// Layout binding
	VkDescriptorSetLayoutBinding textureSamplerLayoutBinding = {};
	textureSamplerLayoutBinding.binding = 1;
	textureSamplerLayoutBinding.descriptorCount = 1;
	textureSamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	textureSamplerLayoutBinding.pImmutableSamplers = nullptr;
	textureSamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
/*** Copy texture to image ***/
	// Start buffer
	VkCommandBuffer *oneTimeBuffer = display->getOneTimeBuffer();
	// Change layout from undefined to transfer
	VkImageMemoryBarrier undefToTransBarrier = {};
	undefToTransBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	undefToTransBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	undefToTransBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	undefToTransBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	undefToTransBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	undefToTransBarrier.image = textureImageVk;
	undefToTransBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	undefToTransBarrier.subresourceRange.baseMipLevel = 0;
	undefToTransBarrier.subresourceRange.levelCount = 1;
	undefToTransBarrier.subresourceRange.baseArrayLayer = 0;
	undefToTransBarrier.subresourceRange.layerCount = 1;
	undefToTransBarrier.srcAccessMask = 0;
	undefToTransBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	vkCmdPipelineBarrier(*oneTimeBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &undefToTransBarrier);
	// Copy texture to image
	VkBufferImageCopy imageCopyRegion = {};
	imageCopyRegion.bufferOffset = 0;
	imageCopyRegion.bufferRowLength = 0;
	imageCopyRegion.bufferImageHeight = 0;
	imageCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageCopyRegion.imageSubresource.mipLevel = 0;
	imageCopyRegion.imageSubresource.baseArrayLayer = 0;
	imageCopyRegion.imageSubresource.layerCount = 1;
	imageCopyRegion.imageOffset = {0, 0, 0};
	imageCopyRegion.imageExtent = {textureWidth, textureHeight, 1};
	vkCmdCopyBufferToImage(*oneTimeBuffer, textureBuffer, textureImageVk, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopyRegion);
	// Change layout from transfer to shader
	VkImageMemoryBarrier transToShadeBarrier = {};
	transToShadeBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	transToShadeBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	transToShadeBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	transToShadeBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	transToShadeBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	transToShadeBarrier.image = textureImageVk;
	transToShadeBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	transToShadeBarrier.subresourceRange.baseMipLevel = 0;
	transToShadeBarrier.subresourceRange.levelCount = 1;
	transToShadeBarrier.subresourceRange.baseArrayLayer = 0;
	transToShadeBarrier.subresourceRange.layerCount = 1;
	transToShadeBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	transToShadeBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	vkCmdPipelineBarrier(*oneTimeBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &transToShadeBarrier);
	display->execOneTimeBuffer(oneTimeBuffer);
	// Clean
	vkDestroyBuffer(display->deviceVk, textureBuffer, nullptr);
    vkFreeMemory(display->deviceVk, textureBufferMemory, nullptr);
/*** Uniform buffer ***/
	// Layout binding
	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	// Buffer
	VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(uniformBufferObject);
    bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    switch(vkCreateBuffer(display->deviceVk, &bufferInfo, nullptr, &uniformBufferVk)){
        case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan buffer layout] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan buffer layout] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		default:
		break;
	}
	// Buffer memory
	VkMemoryRequirements uboMemRequirements;
	vkGetBufferMemoryRequirements(display->deviceVk, uniformBufferVk, &uboMemRequirements);
	VkMemoryAllocateInfo uboMemAllocInfo = {};
	uboMemAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	uboMemAllocInfo.allocationSize = uboMemRequirements.size;
	uboMemAllocInfo.memoryTypeIndex = display->findMemoryType(uboMemRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	if(vkAllocateMemory(display->deviceVk, &uboMemAllocInfo, nullptr, &uniformBufferMemoryVk) != VK_SUCCESS){
		throw "[Vulkan uniform buffer] Error allocate memory";
	}
	vkBindBufferMemory(display->deviceVk, uniformBufferVk, uniformBufferMemoryVk, 0);
	// Init
	uniformBufferObject.model = glm::mat4();
	uniformBufferObject.view = glm::mat4();
	uniformBufferObject.proj = glm::infinitePerspective(glm::radians(user->cameraFOV), (float) display->displayExtentVk.width / (float) display->displayExtentVk.height, 0.1f);
	{
		void* data;
		vkMapMemory(display->deviceVk, uniformBufferMemoryVk, 0, sizeof(uniformBufferObject), 0, &data);
		memcpy(data, &uniformBufferObject, sizeof(uniformBufferObject));
		vkUnmapMemory(display->deviceVk, uniformBufferMemoryVk);
	}
/*** Descriptor pool & set ***/
	// Descriptor pool
	VkDescriptorPoolSize poolSizes[2];
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = 1;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = 1;
	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 2;
	poolInfo.pPoolSizes = poolSizes;
	poolInfo.maxSets = 1;
	if(vkCreateDescriptorPool(display->deviceVk, &poolInfo, nullptr, &descriptorPoolVk) != VK_SUCCESS) {
		throw "[Vulkan descriptor pool] Error create descriptor pool";
	}
	// Descriptor set layout
	VkDescriptorSetLayoutBinding layoutBingings[2] = {uboLayoutBinding, textureSamplerLayoutBinding};
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 2;
	layoutInfo.pBindings = layoutBingings;
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
	// UBO descriptor set
	VkDescriptorSetAllocateInfo descriptorSetAllocInfo = {};
	descriptorSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocInfo.descriptorPool = descriptorPoolVk;
	descriptorSetAllocInfo.descriptorSetCount = 1;
	descriptorSetAllocInfo.pSetLayouts = &descriptorSetLayout;
	if(vkAllocateDescriptorSets(display->deviceVk, &descriptorSetAllocInfo, &descriptorSetVk) != VK_SUCCESS) {
		throw "[Vulkan descriptor set] Error create descriptor set";
	}
	VkDescriptorBufferInfo uniformDescBufferInfo = {};
	uniformDescBufferInfo.buffer = uniformBufferVk;
	uniformDescBufferInfo.offset = 0;
	uniformDescBufferInfo.range = sizeof(uniformBufferObject);
	// Texture descriptor set
	VkDescriptorImageInfo textureDescImageInfo = {};
	textureDescImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	textureDescImageInfo.imageView = textureImageViewVk;
	textureDescImageInfo.sampler = textureSamplerVk;
	// Descriptor write
	VkWriteDescriptorSet descriptorWrites[2];
	descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[0].dstSet = descriptorSetVk;
	descriptorWrites[0].dstBinding = 0;
	descriptorWrites[0].dstArrayElement = 0;
	descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites[0].descriptorCount = 1;
	descriptorWrites[0].pBufferInfo = &uniformDescBufferInfo;
	descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[1].dstSet = descriptorSetVk;
	descriptorWrites[1].dstBinding = 1;
	descriptorWrites[1].dstArrayElement = 0;
	descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrites[1].descriptorCount = 1;
	descriptorWrites[1].pImageInfo = &textureDescImageInfo;
	vkUpdateDescriptorSets(display->deviceVk, 2, descriptorWrites, 0, nullptr);
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
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	// Multisampling
	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	// Color blending
	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
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
	recordCommand(*cmdBuffer, display);
/*** Control ***/
	user->addHandler(this);
}

VkShaderModule SkyBoxModule::createShaderModule(const char *fileName, VkDevice device){
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
	switch(vkCreateShaderModule(device, &createInfo, nullptr, &ret)){
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

void SkyBoxModule::recordCommand(VkCommandBuffer cmdBuffer, Display *display){
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
	beginInfo.pInheritanceInfo = &(display->inheritanceInfoVk);
	// Start command buffer
	vkBeginCommandBuffer(cmdBuffer, &beginInfo);
	vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayoutVk, 0, 1, &descriptorSetVk, 0, nullptr);
	vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelineVk);
	vkCmdDraw(cmdBuffer, 36, 1, 0, 0);
	// End command buffer
	switch(vkEndCommandBuffer(cmdBuffer)){
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

void SkyBoxModule::pointerMotion(Message *message, void *deliver, MessageHandler::DeliverType type, void *data){
	// Get User
	User *user = nullptr;
	if(type == MessageHandler::DeliverType::DEAR_MESSAGE_IPCserver){
		user = (User *)((IPCServer *)deliver)->userData;
	}
	// Get motion
	PointerMotionRequest *request = (PointerMotionRequest *)message;
	float yRotate = request->dx * 0.05;
	float xRotate = -request->dy * 0.05;
	user->cameraRotation += glm::vec3(xRotate, yRotate, 0.0f);
	glm::quat rotationQuat = glm::angleAxis(user->cameraRotation.x, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::angleAxis(user->cameraRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	uniformBufferObject.view = glm::toMat4(rotationQuat);
	// Paint 
	void* memoryData;
	vkMapMemory(user->display->deviceVk, uniformBufferMemoryVk, 0, sizeof(uniformBufferObject), 0, &memoryData);
	memcpy(memoryData, &uniformBufferObject, sizeof(uniformBufferObject));
	vkUnmapMemory(user->display->deviceVk, uniformBufferMemoryVk);
	user->display->paint();
}

void SkyBoxModule::removeModule(){
	delete this;
}

AreaModule *createModule(void *area){
	return new SkyBoxModule((Area *)area);
}