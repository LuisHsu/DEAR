#ifndef GREETER_DEF
#define GREETER_DEF

extern "C"{
	#include <strings.h>
}
#include <fstream>
#include <Server.hpp>
#include <IPCMessage.hpp>
#include <vulkan/vulkan.hpp>

class GreeterClient{
public:
	VkImage vkPresentImage;
	VkFormat vkImageFormat;
	VkExtent2D vkImageExtent;
	VkDeviceMemory vkExportMemory;
	VkImageView vkPresentImageView;
	VkPipelineLayout vkPipelineLayout;
	VkFramebuffer vkFramebuffer;
	VkRenderPass vkRenderPass;
	VkPipeline vkGraphicsPipeline;
	VkCommandPool vkCommandPool;
	VkCommandBuffer vkCommandBuffer;
	VkSemaphore vkImageAvailableSemaphore;
	int displayFd;
	int memoryFd;
	struct sockaddr_un displayDMAddr;
	struct sockaddr_un displayAddr;
};

class Greeter : public AreaServerHandler{
public:
	Greeter();
	~Greeter();
private:
	AreaServer *server;
	VkInstance vkInstance;
	VkDevice vkDevice;
	VkQueue vkGraphicsQueue;
	int32_t vkGraphicsFamily;

	void handleMessage(IPCMessage *message, AreaClient *client);
	void initClient(IPCMessage *message, GreeterClient *client);

	VkResult vkGetMemoryFdKHR(VkDevice device,  const VkMemoryGetFdInfoKHR *pGetFdInfo, int* pFd);
	VkShaderModule createShaderModule(const char *fileName);
};

#endif
