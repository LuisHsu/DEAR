#ifndef DMAREANODE_DEF
#define DMAREANODE_DEF

extern "C"{
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/un.h>
	#include <unistd.h>
	#include <event2/event.h> 
	#include <event2/bufferevent.h>
}

#include <cstring>
#include <cerrno>
#include <iostream>
#include <IPCMessage.hpp>

class AreaNode{
public:
	AreaNode(const char *path);
	~AreaNode();
private:
	evutil_socket_t socketFd;
	struct sockaddr_un serverAddr;
	struct sockaddr_un clientAddr;
	IPCMessageQueue *messageQueue = nullptr;
	struct event_base *eventBase;
	static void socketReceive(evutil_socket_t fd, short event, void *arg);
	static void socketError(struct bufferevent *bev, short event, void *arg);
};

#endif