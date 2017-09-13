#ifndef BACKENDISPLAYVK_DEF
#define BACKENDISPLAYVK_DEF

#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <vulkan/vulkan.hpp>
#include <unistd.h>

#include <Base.hpp>

class BackendDisplayVK : public BackendBase{
public:
	BackendDisplayVK();
	~BackendDisplayVK();
	void run();
private:
	VkInstance vkInstance;
	VkDevice vkDevice;
	VkSurfaceKHR vkSurface;
	VkQueue vkGraphicsQueue;
	VkQueue vkPresentQueue;
	VkSwapchainKHR vkSwapChain;
	VkRenderPass vkRenderPass;
	VkPipelineLayout vkPipelineLayout;
	VkPipeline vkGraphicsPipeline;
	VkCommandPool vkCommandPool;
	VkSemaphore vkImageAvailableSemaphore;
	VkSemaphore vkRenderFinishedSemaphore;
	std::vector<VkImage> vkSwapChainImages;
	std::vector<VkImageView> vkSwapChainImageViews;
	std::vector<VkFramebuffer> vkSwapChainFramebuffers;
	std::vector<VkCommandBuffer> vkCommandBuffers;

	VkShaderModule createShaderModule(const char *fileName);
};

#endif