#ifndef DMAREANODE_DEF
#define DMAREANODE_DEF

extern "C"{
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/un.h>
	#include <unistd.h>
	#include <event2/event.h>
}

#include <cstring>
#include <cerrno>
#include <iostream>
#include <atomic>
#include <thread>
#include <IPCMessage.hpp>
#include <BackendBase.hpp>

class AreaNode{
public:
	AreaNode(const char *path, BackendBase *backend);
	~AreaNode();
private:
	class AreaSocketReceiveArg {
	public:
		AreaNode *areaNode;
		struct event *recvEvent;
	};
	BackendBase *backend;
	evutil_socket_t socketFd;
	struct sockaddr_un serverAddr;
	struct sockaddr_un clientAddr;
	IPCMessageQueue *messageQueue = nullptr;
	struct event_base *eventBase;

	evutil_socket_t displayFd;
	struct sockaddr_un displayAddr;
	std::atomic<bool> isPainting;
	std::thread *paintThread;

	static void socketReceive(evutil_socket_t fd, short event, void *arg);
	void messageHandler(IPCMessage *message);
};

#endif