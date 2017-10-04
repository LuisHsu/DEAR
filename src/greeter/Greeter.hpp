#ifndef GREETER_DEF
#define GREETER_DEF

extern "C"{
	#include <strings.h>
}
#include <fstream>
#include <iostream>
#include <thread>
#include <cstring>
#include <Server.hpp>
#include <IPCMessage.hpp>
#include <vulkan/vulkan.hpp>

class Greeter;

class GreeterClient{
public:
	VkFormat vkImageFormat;
	VkExtent2D vkImageExtent;
	VkImage vkPresentImage;
	VkImage vkStagingImage;
	VkDeviceMemory vkPresentImageMemory;
	VkDeviceMemory vkStagingImageMemory;
	VkImageView vkPresentImageView;
	VkPipelineLayout vkPipelineLayout;
	VkFramebuffer vkFramebuffer;
	VkRenderPass vkRenderPass;
	VkPipeline vkGraphicsPipeline;
	VkCommandPool vkCommandPool;
	VkCommandBuffer vkCommandBuffer;
	VkFence vkStartTransferFence;
	VkEvent vkImageCopyEvent;
	
	uint32_t memSize;
	std::thread *transferThread = nullptr;

	bool isPainting = false;
	static void texelTransfer(AreaClient *client, Greeter *greeter);
};

class Greeter : public AreaServerHandler{
	friend GreeterClient;
public:
	Greeter();
	~Greeter();
private:
	AreaServer *server;
	VkInstance vkInstance;
	VkPhysicalDevice vkPhyDevice;
	VkDevice vkDevice;
	VkQueue vkGraphicsQueue;
	int32_t vkGraphicsFamily;

	void handleMessage(IPCMessage *message, AreaClient *client);
	void initClient(IPCMessage *message, AreaClient *client);
	void paint(GreeterClient *client);
	VkShaderModule createShaderModule(const char *fileName);
	int32_t findMemoryType(uint32_t memoryTypeBits, VkMemoryPropertyFlags properties);
};

#endif
