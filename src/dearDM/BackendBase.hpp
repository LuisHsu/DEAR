#ifndef BACKENDBASE_DEF
#define BACKENDBASE_DEF

#include <iostream>
#include <vector>
#include <fstream>
#include <vulkan/vulkan.hpp>

class BackendBase{
public:
	enum SurfType {XCB, DISPLAY};
	SurfType surf;

	BackendBase(SurfType surf);
	virtual ~BackendBase();
	virtual void paint() = 0;
	virtual void initTexture(int fd) = 0;

	VkInstance vkInstance;
	VkDevice vkDevice;
	VkSurfaceKHR vkSurface;
	VkQueue vkGraphicsQueue;
	VkQueue vkPresentQueue;
	VkSwapchainKHR vkSwapChain;
	VkRenderPass vkRenderPass;
	VkPipeline vkGraphicsPipeline;
	VkCommandPool vkCommandPool;
	VkExtent2D vkDisplayExtent;
	VkSurfaceFormatKHR vkSurfaceFormat;
	VkSemaphore vkImageAvailableSemaphore;
	VkSemaphore vkRenderFinishedSemaphore;
	VkImage vkTextureImage;
	VkDeviceMemory vkTextureMemory;
	VkImageView vkTextureImageView;
	VkSampler vkTextureSampler;
	VkDescriptorPool vkDescriptorPool;
	VkDescriptorSet vkDescriptorSet;
	VkPipelineLayout vkPipelineLayout;
	std::vector<VkImage> vkSwapChainImages;
	std::vector<VkImageView> vkSwapChainImageViews;
	std::vector<VkFramebuffer> vkSwapChainFramebuffers;
	std::vector<VkCommandBuffer> vkCommandBuffers;
	VkShaderModule createShaderModule(const char *fileName);
};

#endif