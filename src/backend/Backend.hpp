#ifndef BACKEND_DEF
#define BACKEND_DEF

#include <iostream>
#include <cstring>
#include <vector>

extern "C"{
	#include <fcntl.h>
	#include <xf86drm.h>
	#include <xf86drmMode.h>
}

#ifdef USE_VULKAN
#include <vulkan/vulkan.hpp>
#endif

class Backend{
public:
	enum LibType {AUTO, VULKAN, OPENGL};
	LibType lib;

	Backend(LibType lib);
	~Backend();

	#ifdef USE_VULKAN
	VkInstance vkInstance;
	VkDevice vkDevice;
	VkSurfaceKHR vkSurface;
	VkQueue vkGraphicsQueue;
	VkQueue vkPresentQueue;
	VkSwapchainKHR vkSwapChain;
	VkExtent2D vkDisplayExtent;
	VkSurfaceFormatKHR vkSurfaceFormat;
	std::vector<VkImage> vkSwapChainImages;
	void initVK();
	#endif

	int drmFd;
	drmModeRes *resources;
	void initGL();
};

#endif