#ifndef DEAR_DISPLAY
#define DEAR_DISPLAY

#include <vulkan/vulkan.hpp>

class Display{
public:
	enum DisplayType {DEAR_DISPLAY_XCB, DEAR_DISPLAY_DIRECT};
	DisplayType type;
	void init();
	void paint(bool needRecord = false);
	float r;
	float g;
	float b;
protected:
	VkInstance instanceVk;
	VkPhysicalDevice phyDeviceVk;
	VkExtent2D displayExtentVk;
	VkSurfaceKHR surfaceVk;
	VkDevice deviceVk;
	VkQueue graphicQueueVk;
	VkQueue presentQueueVk;
	VkSurfaceFormatKHR surfaceFormatVk;
	VkPresentModeKHR presentModeVk;
	VkSwapchainKHR swapChainVk;
	VkCommandPool commandPoolVk;
	VkRenderPass renderPassVk;
	VkSemaphore imageAvailableSemaphoreVk;
	VkSemaphore renderFinishedSemaphoreVk;
	std::vector<VkImage> swapChainImagesVk;
	std::vector<VkImageView> swapChainImageViewsVk;
	std::vector<VkFramebuffer> swapChainFramebuffersVk;
	std::vector<VkCommandBuffer> commandBuffersVk;
	int32_t graphicFamily = -1;
	int32_t presentFamily = -1;
	void record(uint32_t index);
};

#endif