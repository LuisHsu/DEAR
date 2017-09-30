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
#include <list>
#include <IPCMessage.hpp>


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
		AreaServerHandler *handler
	);
	void *data = nullptr;
private:
	uint32_t index;
	int clientFd;
	struct sockaddr_un addr;
	uint32_t addrlen;
	AreaServer *server;
	struct event *receiveEvent;
	AreaServerHandler *messageHandler;
	static void socketReceive(evutil_socket_t fd, short event, void *arg);
};

class AreaServerHandler{
public:
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