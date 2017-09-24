#ifndef AREASERVER_DEF
#define AREASERVER_DEF

extern "C"{
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/un.h>
	#include <unistd.h>
	#include <event2/event.h>
}
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <vector>
#include <IPCMessage.hpp>

class AreaServerHandler{
public:
	virtual void handleMessage(IPCMessage *message) = 0;
};

class AreaServer{
public:
	AreaServer(const char *path);
	~AreaServer();
	void setHandler(AreaServerHandler *handler);
	void start();
private:
	class AreaSocketReceiveArg {
	public:
		AreaServer *areaServer;
		struct event *receiveEvent;
	};
	int listenFd;
	int clientFd;
	uint32_t clientAddrlen;
	struct sockaddr_un listenAddr;
	struct sockaddr_un clientAddr;
	struct event_base *eventBase;
	AreaServerHandler *messageHandler;
	static void socketAccept(evutil_socket_t fd, short event, void *arg);
	static void socketReceive(evutil_socket_t fd, short event, void *arg);
};

#endif