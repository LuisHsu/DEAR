#ifndef BACKENDXCBVK_DEF
#define BACKENDXCBVK_DEF

#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <vulkan/vulkan.hpp>
extern "C"{
#include <xcb/xcb.h>
#include <unistd.h>
}

#include <Base.hpp>

class BackendXcbVK : public BackendBase{
public:
	BackendXcbVK();
	~BackendXcbVK();
	void run();
private:
	xcb_connection_t *xcbConnection = nullptr;
	xcb_window_t xcbWindow;

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