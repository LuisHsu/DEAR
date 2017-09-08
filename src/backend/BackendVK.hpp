#ifndef BACKENDVK_DEF
#define BACKENDVK_DEF

#include <iostream>
#include <cstring>
#include <vector>
#include <vulkan/vulkan.hpp>

#include <Base.hpp>

class BackendVK : public BackendBase{
public:
	BackendVK();
	~BackendVK();
private:
	VkInstance vkInstance;
	VkDevice vkDevice;
	VkSurfaceKHR vkSurface;
	VkQueue vkGraphicsQueue;
	VkQueue vkPresentQueue;
	VkSwapchainKHR vkSwapChain;
	VkExtent2D vkDisplayExtent;
	VkSurfaceFormatKHR vkSurfaceFormat;
	std::vector<VkImage> vkSwapChainImages;
	std::vector<VkImageView> vkSwapChainImageViews;

	VkShaderModule createShaderModule(const char *fileName);
};

#endif