extern "C"{
	#include <uv.h>
	#include <termios.h>
}
#include <iostream>
#include <message/IPCClient.hpp>
#include "session.hpp"
#include "input.hpp"

int main(void){
	// Termios
	struct termios term;
	tcgetattr(STDIN_FILENO, &term);
	term.c_lflag &= ~(ICANON | ECHO | ECHOE);
	term.c_oflag &= ~OPOST;
	tcsetattr(STDIN_FILENO, TCSANOW, &term);
	// libuv loop
	uv_loop_t loop;
	uv_loop_init(&loop);
	// Session
	Session session;
	// IPC Client
	IPCClient client(&loop, &session);
	client.start("/tmp/dear/dear-area");
	// Input
	Input input(&loop, &client);
	// Run loop
	uv_run(&loop, UV_RUN_DEFAULT);
	uv_loop_close(&loop);
	// Flush and reset termio
	term.c_lflag |= ICANON | ECHO | ECHOE;
	term.c_oflag |= OPOST;
	tcflush(STDIN_FILENO, TCIFLUSH);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
	return 0;
}
