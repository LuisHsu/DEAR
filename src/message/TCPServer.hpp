#ifndef DEAR_TCPSERVER_DEF
#define DEAR_TCPSERVER_DEF

extern "C"{
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <uv.h>
}
#include <iostream>
#include <cstdlib>
#include <vector>
#include <list>
#include <cstring>

#include "message.hpp"

class TCPServer{
public:
	TCPServer(uv_loop_t *loop, const char *name, const char *service, MessageHandler *handler);
	~TCPServer();
	void start();
private:
	uv_loop_t *uvLoop;
	uv_tcp_t serverSocket;
	struct addrinfo *serverAddr;
	std::vector<char> messageBuf;
	std::list<uv_stream_t *> clientStreams;
	MessageHandler *handler;
};

#endif