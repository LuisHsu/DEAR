#include "display.hpp"

void Display::localInit(){
/*** Swap chain ***/
	// Get surface format
	uint32_t surfaceFormatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(phyDeviceVk, surfaceVk, &surfaceFormatCount, nullptr);
	VkSurfaceFormatKHR surfaceFormats[surfaceFormatCount];
	vkGetPhysicalDeviceSurfaceFormatsKHR(phyDeviceVk, surfaceVk, &surfaceFormatCount, surfaceFormats);
	// Pick surface format
	if (surfaceFormatCount == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED) {
		surfaceFormatVk.format = VK_FORMAT_B8G8R8A8_SRGB;
		surfaceFormatVk.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	}else{
		surfaceFormatVk.format = surfaceFormats[0].format;
		surfaceFormatVk.colorSpace = surfaceFormats[0].colorSpace;
		for(uint32_t i = 0; i < surfaceFormatCount; ++i){
			if(surfaceFormats[i].format == VK_FORMAT_A8B8G8R8_SRGB_PACK32 || surfaceFormats[i].format == VK_FORMAT_R8G8B8A8_SRGB || surfaceFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB){
				surfaceFormatVk.format = surfaceFormats[i].format;
				surfaceFormatVk.colorSpace = surfaceFormats[i].colorSpace;
				break;
			}
		}
	}
	// Get present mode
	uint32_t presentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(phyDeviceVk, surfaceVk, &presentModeCount, nullptr);
	VkPresentModeKHR presentModes[presentModeCount];
	vkGetPhysicalDeviceSurfacePresentModesKHR(phyDeviceVk, surfaceVk, &presentModeCount, presentModes);
	for(uint32_t i = 0; i < presentModeCount; ++i){
		if(presentModes[i] == VK_PRESENT_MODE_FIFO_KHR){
			presentModeVk = VK_PRESENT_MODE_FIFO_KHR;
			break;
		}else if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            presentModeVk = VK_PRESENT_MODE_MAILBOX_KHR;
        }else if (presentModes[i] != VK_PRESENT_MODE_IMMEDIATE_KHR) {
            presentModeVk = presentModes[i];
        }
	}
	// Swapchain capability
	VkSurfaceCapabilitiesKHR surfaceCapability;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(phyDeviceVk, surfaceVk, &surfaceCapability);
	// Create swap chain
	VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
	swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapChainCreateInfo.surface = surfaceVk;
	swapChainCreateInfo.minImageCount = (surfaceCapability.maxImageCount > 0 && (surfaceCapability.minImageCount + 1 > surfaceCapability.maxImageCount)) ? surfaceCapability.maxImageCount : surfaceCapability.minImageCount + 1;
	swapChainCreateInfo.imageFormat = surfaceFormatVk.format;
	swapChainCreateInfo.imageColorSpace = surfaceFormatVk.colorSpace;
	swapChainCreateInfo.imageExtent = displayExtentVk;
	swapChainCreateInfo.imageArrayLayers = 1;
	swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	if (graphicFamily != presentFamily) {
		swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapChainCreateInfo.queueFamilyIndexCount = 2;
		uint32_t queueFamilyIndices[] = {(uint32_t)graphicFamily, (uint32_t)presentFamily};
		swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
	} else {
		swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapChainCreateInfo.queueFamilyIndexCount = 0;
		swapChainCreateInfo.pQueueFamilyIndices = nullptr;
	}
	swapChainCreateInfo.preTransform = surfaceCapability.currentTransform;
	swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapChainCreateInfo.presentMode = presentModeVk;
	swapChainCreateInfo.clipped = VK_TRUE;
	swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
	switch(vkCreateSwapchainKHR(deviceVk, &swapChainCreateInfo, nullptr, &swapChainVk)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan swapchain] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan swapchain] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		case VK_ERROR_DEVICE_LOST:
			throw "[Vulkan swapchain] VK_ERROR_DEVICE_LOST";
		break;
		case VK_ERROR_SURFACE_LOST_KHR:
			throw "[Vulkan swapchain] VK_ERROR_SURFACE_LOST_KHR";
		break;
		case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
			throw "[Vulkan swapchain] VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
		break;
		default:
		break;
	}
	uint32_t swapchainImageCount = 0;
	vkGetSwapchainImagesKHR(deviceVk, swapChainVk, &swapchainImageCount, nullptr);
	swapChainImagesVk.resize(swapchainImageCount);
	vkGetSwapchainImagesKHR(deviceVk, swapChainVk, &swapchainImageCount, swapChainImagesVk.data());
	updatePrimary.resize(swapchainImageCount, true);
/*** Image views ***/
	swapChainImageViewsVk.resize(swapchainImageCount);
	for (size_t i = 0; i < swapChainImagesVk.size(); i++) {
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swapChainImagesVk[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = surfaceFormatVk.format;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		if(vkCreateImageView(deviceVk, &createInfo, nullptr, &swapChainImageViewsVk[i])!= VK_SUCCESS){
			throw "[Vulkan image view] Error create image view.";
		}
	}
/*** Render pass ***/
	// Color attachment
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = surfaceFormatVk.format;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	// Color attachment ref
	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	// Subpass description
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	// Render pass
	VkRenderPassCreateInfo renderPassCreateInfo = {};
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.attachmentCount = 1;
	renderPassCreateInfo.pAttachments = &colorAttachment;
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpass;
	renderPassCreateInfo.dependencyCount = 0;
	switch(vkCreateRenderPass(deviceVk, &renderPassCreateInfo, nullptr, &renderPassVk)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan render pass] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan render pass] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		default:
		break;
	}
/*** Frame buffer ***/
	swapChainFramebuffersVk.resize(swapchainImageCount);
	for (size_t i = 0; i < swapChainImageViewsVk.size(); i++) {
		VkImageView attachments[] = {swapChainImageViewsVk[i]};
		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPassVk;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = displayExtentVk.width;
		framebufferInfo.height = displayExtentVk.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(deviceVk, &framebufferInfo, nullptr, &swapChainFramebuffersVk[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
/*** Command pool ***/
	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = graphicFamily;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	switch(vkCreateCommandPool(deviceVk, &poolInfo, nullptr, &commandPoolVk)){
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
	commandBuffersVk.resize(swapchainImageCount);
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPoolVk;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = swapchainImageCount;
	switch(vkAllocateCommandBuffers(deviceVk, &allocInfo, commandBuffersVk.data())){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan command buffer] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan command buffer] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		default:
		break;
	}
/*** Command buffer inheritance info ***/
	inheritanceInfoVk.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
	inheritanceInfoVk.renderPass = renderPassVk;
	inheritanceInfoVk.subpass = 0;
	inheritanceInfoVk.framebuffer = VK_NULL_HANDLE;
	inheritanceInfoVk.occlusionQueryEnable = VK_FALSE;
/*** Create semaphores ***/
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	switch(vkCreateSemaphore(deviceVk, &semaphoreInfo, nullptr, &imageAvailableSemaphoreVk)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan image available semaphore] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan image available semaphore] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		default:
		break;
	}
	switch(vkCreateSemaphore(deviceVk, &semaphoreInfo, nullptr, &renderFinishedSemaphoreVk)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan render finished semaphore] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan render finished semaphore] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		default:
		break;
	}
/*** paint ***/
	paint();
}
void Display::paint(){
	uint32_t imageIndex;
	vkAcquireNextImageKHR(deviceVk, swapChainVk, std::numeric_limits<uint32_t>::max(), imageAvailableSemaphoreVk, VK_NULL_HANDLE, &imageIndex);
	// Record
	if(updatePrimary[imageIndex]){
		primaryRecord(imageIndex);
		updatePrimary[imageIndex] = false;
	}
	// Submit command buffer
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &imageAvailableSemaphoreVk;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffersVk[imageIndex];
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &renderFinishedSemaphoreVk;
	switch(vkQueueSubmit(graphicQueueVk, 1, &submitInfo, VK_NULL_HANDLE)){
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
	// Present
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &renderFinishedSemaphoreVk;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapChainVk;
	presentInfo.pImageIndices = &imageIndex;
	switch(vkQueuePresentKHR(presentQueueVk, &presentInfo)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan queue present] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan queue present] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		case VK_ERROR_DEVICE_LOST:
			throw "[Vulkan queue present] VK_ERROR_DEVICE_LOST";
		break;
		case VK_ERROR_OUT_OF_DATE_KHR:
			throw "[Vulkan queue present] VK_ERROR_OUT_OF_DATE_KHR";
		break;
		case VK_ERROR_SURFACE_LOST_KHR:
			throw "[Vulkan queue present] VK_ERROR_SURFACE_LOST_KHR";
		break;
		default:
		break;
	}	
}
void Display::primaryRecord(uint32_t index){
/*** Command buffer record ***/
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	beginInfo.pInheritanceInfo = nullptr;
	// Start command buffer
	vkBeginCommandBuffer(commandBuffersVk[index], &beginInfo);
	// Begin render pass
	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPassVk;
	renderPassInfo.framebuffer = swapChainFramebuffersVk[index];
	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent = displayExtentVk;
	VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;
	vkCmdBeginRenderPass(commandBuffersVk[index], &renderPassInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
	if(secondaryCommandBuffersVk.size() > 0){
		vkCmdExecuteCommands(commandBuffersVk[index], secondaryCommandBuffersVk.size(), secondaryCommandBuffersVk.data());
	}
	vkCmdEndRenderPass(commandBuffersVk[index]);
	switch(vkEndCommandBuffer(commandBuffersVk[index])){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan command buffer record] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan command buffer record] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		default:
		break;
	}
}
VkCommandBuffer *Display::getSecCmdBuffer(){
	VkCommandBuffer newCmdBuffer = {};
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPoolVk;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
	allocInfo.commandBufferCount = 1;
	switch(vkAllocateCommandBuffers(deviceVk, &allocInfo, &newCmdBuffer)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan command buffer] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan command buffer] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		default:
		break;
	}
	secondaryCommandBuffersVk.push_back(newCmdBuffer);
	return &(secondaryCommandBuffersVk.back());
}
VkCommandBuffer *Display::getOneTimeBuffer(){
	VkCommandBuffer *newCmdBuffer = new VkCommandBuffer;
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPoolVk;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;
	switch(vkAllocateCommandBuffers(deviceVk, &allocInfo, newCmdBuffer)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan command buffer] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan command buffer] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		default:
		break;
	}
	VkCommandBufferBeginInfo copyBeginInfo = {};
    copyBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    copyBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(*newCmdBuffer, &copyBeginInfo);
	return newCmdBuffer;
}
void Display::execOneTimeBuffer(VkCommandBuffer *commandBuffer){
	vkEndCommandBuffer(*commandBuffer);
	// Fence
	VkFence fence;
	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	vkCreateFence(deviceVk, &fenceInfo, nullptr, &fence);
	// Submit
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = commandBuffer;
	vkQueueSubmit(graphicQueueVk, 1, &submitInfo, fence);
	while(vkWaitForFences(deviceVk, 1, &fence, VK_FALSE, 3000000) == VK_TIMEOUT);
	vkDestroyFence(deviceVk, fence, nullptr);
	vkFreeCommandBuffers(deviceVk, commandPoolVk, 1, commandBuffer);
	delete commandBuffer;
}
void Display::update(){
	for(uint32_t i = 0 ; i < updatePrimary.size(); ++i){
		updatePrimary[i] = true;
	}
}
uint32_t Display::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties){
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(phyDeviceVk, &memProperties);
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}
	throw "failed to find suitable memory type!";
}
GrVkBackendContext *Display::createSkiaContext(){
	GrVkBackendContext *context = new GrVkBackendContext;
	context->fInstance = instanceVk;
    context->fPhysicalDevice = phyDeviceVk;
    context->fDevice = deviceVk;
    context->fQueue = graphicQueueVk;
    context->fGraphicsQueueIndex = graphicFamily;
    context->fMinAPIVersion = VK_API_VERSION_1_0;
    context->fExtensions = kEXT_debug_report_GrVkExtensionFlag | kKHR_surface_GrVkExtensionFlag;
    context->fInterface = sk_sp<GrVkInterface>(new GrVkInterface(vkGetInstanceProcAddr, vkGetDeviceProcAddr, instanceVk, deviceVk, kEXT_debug_report_GrVkExtensionFlag | kKHR_surface_GrVkExtensionFlag));
	return context;
}