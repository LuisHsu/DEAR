#ifndef IPCMESSAGE_DEF
#define IPCMESSAGE_DEF

#include <mutex>
#include <condition_variable>
#include <vulkan/vulkan.hpp>
#include <queue>

enum IPCType{
	IPC_Connect,
	IPC_Frame
};

class IPCMessage{
public:
	IPCType type;
	int32_t length;
};

class IPCMessageQueue{
public:
	IPCMessageQueue(uint32_t max);
	void produce(IPCMessage &message);
	IPCMessage consume();
private:
	uint32_t max;
	std::mutex queueMutex;
	std::condition_variable fullCond;
	std::condition_variable emptyCond;
	std::queue<IPCMessage> msgQueue;
};

class IPCConnectMessage: public IPCMessage{
public:
	VkFormat format;
	VkExtent2D extent;
};

class IPCFrameMessage: public IPCMessage{
public:
	char* dataBytes;
};

#endif