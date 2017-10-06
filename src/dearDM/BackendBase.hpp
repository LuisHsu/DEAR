#ifndef BACKENDBASE_DEF
#define BACKENDBASE_DEF

#include <iostream>
#include <vector>
#include <fstream>
#include <IPCMessage.hpp>
#include <vulkan/vulkan.hpp>

class BackendBase{
public:
	enum SurfType {XCB, DISPLAY};
	SurfType surf;

	BackendBase(SurfType surf);
	virtual ~BackendBase();
	virtual void paint(IPCFrameMessage *message) = 0;
	virtual void initTexture() = 0;

	VkPhysicalDevice vkPhyDevice;
	VkInstance vkInstance;
	VkDevice vkDevice;
	VkSurfaceKHR vkSurface;
	VkQueue vkGraphicsQueue;
	VkQueue vkPresentQueue;
	VkSwapchainKHR vkSwapChain;
	VkCommandPool vkCommandPool;
	VkExtent2D vkDisplayExtent;
	VkSurfaceFormatKHR vkSurfaceFormat;
	VkSemaphore vkImageAvailableSemaphore;
	VkSemaphore vkRenderFinishedSemaphore;
	VkImage vkTextureImage;
	VkDeviceMemory vkTextureMemory;
	VkFence vkMapTextureFence;
	std::vector<VkImage> vkSwapChainImages;
	std::vector<VkCommandBuffer> vkCommandBuffers;
	int32_t memSize;
	int32_t findMemoryType(uint32_t memoryTypeBits, VkMemoryPropertyFlags properties);
};

#endif