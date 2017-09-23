#ifndef AREASERVER_DEF
#define AREASERVER_DEF

extern "C"{
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/un.h>
	#include <unistd.h>
}
#include <iostream>
#include <cstring>
#include <cerrno>
#include <vector>
#include <IPCMessage.hpp>

typedef void (*AreaServerHandler)(IPCMessage *);

class AreaServer{
public:
	AreaServer(const char *path);
	~AreaServer();
	void setHandler(AreaServerHandler handler);
	void start();
private:
	int listenFd;
	int clientFd;
	uint32_t clientAddrlen;
	struct sockaddr_un listenAddr;
	struct sockaddr_un clientAddr;
	AreaServerHandler messageHandler;
};

#endif