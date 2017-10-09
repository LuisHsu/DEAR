#ifndef IPCCLIENT_DEF
#define IPCCLIENT_DEF

extern "C"{
	#include <uv.h>
}
#include <cstdlib>
#include <iostream>
#include <vector>
#include <cstring>
#include <message.hpp>

class IPCClient{
public:
	IPCClient(uv_loop_t *loop, MessageHandler *handler);
	~IPCClient();
	void start(const char *path);
private:
	uv_loop_t *uvLoop;
	uv_pipe_t clientPipe;
	uv_connect_t uvConnect;
	MessageHandler *handler;
	std::vector<char> messageBuf;
};

#endif