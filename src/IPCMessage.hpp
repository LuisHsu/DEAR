#ifndef IPCMESSAGE_DEF
#define IPCMESSAGE_DEF

#include <mutex>
#include <condition_variable>
#include <queue>

enum IPCType{
	IPC_Connect
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

#endif