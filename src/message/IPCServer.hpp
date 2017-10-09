#ifndef IPCSERVER_DEF
#define IPCSERVER_DEF

extern "C"{
	#include <uv.h>
}
#include <cstdlib>
#include <iostream>
#include <vector>
#include <cstring>
#include <message.hpp>

class IPCServer{
public:
	IPCServer(uv_loop_t *loop, const char *path, MessageHandler *handler);
	~IPCServer();
	void start();
private:
	uv_loop_t *uvLoop;
	uv_pipe_t serverPipe;
	uv_pipe_t clientPipe;
	MessageHandler *handler;
	std::vector<char> messageBuf;
};

#endif