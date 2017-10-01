#ifndef GREETER_DEF
#define GREETER_DEF

extern "C"{
	#include <strings.h>
}

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
	void handleMessage(IPCMessage *message, AreaClient *client);
	void initClient(IPCMessage *message, GreeterClient *client);

	VkResult vkGetMemoryFdKHR(VkDevice device,  const VkMemoryGetFdInfoKHR *pGetFdInfo, int* pFd);
};

#endif
