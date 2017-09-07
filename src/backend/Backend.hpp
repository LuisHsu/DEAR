#ifndef BACKEND_DEF
#define BACKEND_DEF

#include <iostream>
#include <cstring>
#include <vector>

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
	VkPhysicalDevice vkPhyDevice;
	VkDevice vkDevice;
	VkDisplayKHR vkDisplay;
	VkDisplayPropertiesKHR vkDisplayProperties;
	VkDisplayModeKHR vkDisplayMode;
	VkSurfaceKHR vkSurface;
	VkQueue vkGraphicsQueue;
	VkQueue vkPresentQueue;
	VkSurfaceFormatKHR vkSurfaceFormat;

	void initVK();
	#endif

	//void initGL();
};

#endif