#ifndef DEAR_DISPLAY
#define DEAR_DISPLAY

#include <vector>
#include <skia.h>

class Display{
public:
	enum DisplayType {DEAR_DISPLAY_XCB, DEAR_DISPLAY_DIRECT};
	
	DisplayType type;
	VkExtent2D displayExtentVk;
	VkSurfaceFormatKHR surfaceFormatVk;
	VkDevice deviceVk;
	VkRenderPass renderPassVk;
	VkCommandBufferInheritanceInfo inheritanceInfoVk;
	void localInit();
	void update();
	VkCommandBuffer *getSecCmdBuffer();
	VkCommandBuffer *getOneTimeBuffer();
	void execOneTimeBuffer(VkCommandBuffer *buffer);
	void paint();
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	GrVkBackendContext *createSkiaContext();

protected:
	VkInstance instanceVk;
	VkPhysicalDevice phyDeviceVk;
	VkSurfaceKHR surfaceVk;
	VkQueue graphicQueueVk;
	VkQueue presentQueueVk;
	VkPresentModeKHR presentModeVk;
	VkSwapchainKHR swapChainVk;
	VkCommandPool commandPoolVk;
	VkSemaphore imageAvailableSemaphoreVk;
	VkSemaphore renderFinishedSemaphoreVk;
	std::vector<VkImage> swapChainImagesVk;
	std::vector<VkImageView> swapChainImageViewsVk;
	std::vector<VkFramebuffer> swapChainFramebuffersVk;
	std::vector<VkCommandBuffer> commandBuffersVk;
	std::vector<VkCommandBuffer> secondaryCommandBuffersVk;
	std::vector<bool> updatePrimary;
	int32_t graphicFamily = -1;
	int32_t presentFamily = -1;
	void primaryRecord(uint32_t index);
};

#endif