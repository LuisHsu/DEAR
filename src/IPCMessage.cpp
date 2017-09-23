#include <IPCMessage.hpp>

IPCMessageQueue::IPCMessageQueue(uint32_t max):max(max){
}

void IPCMessageQueue::produce(IPCMessage &message){
	std::unique_lock<std::mutex> queueLock(queueMutex);
	while(msgQueue.size() >= max){
		fullCond.wait(queueLock);
	}
	msgQueue.push(message);
	emptyCond.notify_all();
}

IPCMessage IPCMessageQueue::consume(){
	std::unique_lock<std::mutex> queueLock(queueMutex);
	while(msgQueue.size() == 0){
		emptyCond.wait(queueLock);
	}
	IPCMessage ret = msgQueue.front();
	msgQueue.pop();
	fullCond.notify_all();
	return ret;
}