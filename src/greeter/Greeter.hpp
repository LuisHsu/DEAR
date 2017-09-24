#ifndef GREETER_DEF
#define GREETER_DEF

#include <Server.hpp>
#include <IPCMessage.hpp>
#include <vulkan/vulkan.hpp>

class Greeter : public AreaServerHandler{
public:
	Greeter(AreaServer *server);
	~Greeter();
private:
	AreaServer *server;
	VkInstance vkInstance;
	VkDevice vkDevice;
	VkQueue vkGraphicsQueue;
	VkImage vkPresentImage;
	VkFormat vkImageFormat;
	VkExtent2D vkImageExtent;
	void handleMessage(IPCMessage *message);
	void initVulkan(IPCMessage *message);
};

#endif
