#ifndef TCPSERVER_DEF
#define TCPSERVER_DEF

extern "C"{
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/un.h>
	#include <uv.h>
}
#include <cstdlib>
#include <vector>
#include <cstring>
#include <message.hpp>

class TCPServer{
public:
	TCPServer(uv_loop_t *loop, const char *path);
	~TCPServer();
	void start(MessageHandler *handler);
private:
	uv_loop_t *loop;
	uv_tcp_t uvPipe;
	std::vector<char> messageBuf;
};

#endif