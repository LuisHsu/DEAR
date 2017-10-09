#ifndef IPCSERVER_DEF
#define IPCSERVER_DEF

extern "C"{
	#include <unistd.h>
	#include <uv.h>
}
#include <cstdlib>
#include <vector>
#include <cstring>
#include <message.hpp>

class IPCServer{
public:
	IPCServer(uv_loop_t *loop, const char *path);
	~IPCServer();
	void start(MessageHandler *handler);
private:
	uv_loop_t *loop;
	uv_pipe_t uvPipe;
	std::vector<char> messageBuf;
};

#endif