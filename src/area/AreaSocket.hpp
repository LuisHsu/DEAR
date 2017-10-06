#ifndef AREASOCKET_DEF
#define AREASOCKET_DEF

extern "C"{
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/un.h>
	#include <unistd.h>
	#include <event2/event.h>
	#include <semaphore.h>
}
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <vector>
#include <thread>
#include <list>
#include <IPCMessage.hpp>
#include <vulkan/vulkan.hpp>

class AreaServer;
class AreaServerHandler;

class AreaClient{
public:
	AreaClient(
		uint32_t index,
		int fd,
		struct sockaddr_un addr,
		uint32_t addrlen,
		AreaServer *server,
		struct event_base *eventBase,
		AreaServerHandler *area
	);
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
	sem_t transferFinishedSem;
	uint32_t memSize;
	std::thread *transferThread = nullptr;
	bool isPainting = false;
	static void texelTransfer(AreaClient *client, AreaServerHandler *area);
	void *data = nullptr;
	ssize_t sendMessage(IPCMessage *message);
	void initClient(IPCMessage *message);
private:
	uint32_t index;
	int clientFd;
	struct sockaddr_un addr;
	uint32_t addrlen;
	AreaServer *server;
	struct event *receiveEvent;
	AreaServerHandler *area;
	static void socketReceive(evutil_socket_t fd, short event, void *arg);
	VkShaderModule createShaderModule(const char *fileName);
	int32_t findMemoryType(VkPhysicalDevice phyDevice, uint32_t memoryTypeBits, VkMemoryPropertyFlags properties);
};

class AreaServerHandler{
public:
	AreaServerHandler();
	AreaServer *server;
	VkInstance vkInstance;
	VkPhysicalDevice vkPhyDevice;
	VkDevice vkDevice;
	VkQueue vkGraphicsQueue;
	int32_t vkGraphicsFamily;
	virtual void handleMessage(IPCMessage *message, AreaClient *client) = 0;
};

class AreaServer{
public:
	AreaServer(const char *path);
	~AreaServer();
	void start(AreaServerHandler *handler);
	void eraseClient(uint32_t index);
private:
	int listenFd;
	struct sockaddr_un listenAddr;
	struct event_base *eventBase;
	struct event *acceptEvent;
	AreaServerHandler *handler;
	std::list<AreaClient *> clients;
	static void socketAccept(evutil_socket_t fd, short event, void *arg);
};

#endif