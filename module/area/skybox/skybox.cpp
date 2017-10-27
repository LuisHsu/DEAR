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
/*** Uniform buffer ***/
	// Descriptor
	/*
	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &uboLayoutBinding;
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
	// Buffer
	VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(UniformBufferObject);
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
    }*/
/*** Fixed function ***/
	// Vertex input
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 0;
	vertexInputInfo.vertexAttributeDescriptionCount = 0;
	// Input assembly
	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
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
	// Pipeline layout
	VkPipelineLayout pipelineLayout;
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;	
	switch(vkCreatePipelineLayout(display->deviceVk, &pipelineLayoutInfo, nullptr, &pipelineLayout)) {
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan pipeline layout] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan pipeline layout] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		default:
		break;
	}
/*** Pipeline ***/
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
	pipelineInfo.layout = pipelineLayout;
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
	vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelineVk);
	vkCmdDraw(cmdBuffer, 3, 1, 0, 0);
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

void SkyBoxModule::removeModule(){
	delete this;
}

AreaModule *createModule(void *area){
	return new SkyBoxModule((Area *)area);
}