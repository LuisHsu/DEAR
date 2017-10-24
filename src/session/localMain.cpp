extern "C"{
	#include <uv.h>
	#include <termios.h>
}
#include <iostream>
#include <cstdlib>
#include <message/IPCClient.hpp>
#include "session.hpp"
#include "input.hpp"

int main(void){
	// Termios
	struct termios term = {};
	tcgetattr(STDIN_FILENO, &term);
	{
		struct termios rawterm = term;
		cfmakeraw(&rawterm);
		rawterm.c_oflag |= OPOST;
		tcsetattr(STDIN_FILENO, TCSANOW, &rawterm);
	}
	// libuv loop
	uv_loop_t *loop = (uv_loop_t *)malloc(sizeof(uv_loop_t));
	uv_loop_init(loop);
	// Session
	Session session;
	// IPC Client
	IPCClient client(loop, &session);
	client.start("/tmp/dear/dear-area");
	// Input
	Input input(loop, &client);
	// Run loop
	uv_run(loop, UV_RUN_DEFAULT);
	// Clean loop
	uv_loop_close(loop);
	free(loop);
	// Reset term & clean stdin
	tcflush(STDIN_FILENO, TCIFLUSH);
	tcsetattr(STDIN_FILENO, TCSANOW, &term);
	return 0;
}
