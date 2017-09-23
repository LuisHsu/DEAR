#ifndef DMAREANODE_DEF
#define DMAREANODE_DEF

extern "C"{
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/un.h>
	#include <unistd.h>
}

#include <cstring>
#include <cerrno>
#include <iostream>
#include <IPCMessage.hpp>

class AreaNode{
public:
	AreaNode(const char *path);
	~AreaNode();
	void run();
private:
	int socketFd;
	struct sockaddr_un serverAddr;
	struct sockaddr_un clientAddr;
	ssize_t sendMsg(IPCMessage *message);
};

#endif