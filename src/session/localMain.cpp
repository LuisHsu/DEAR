extern "C"{
	#include <uv.h>
}
#include <iostream>
#include <message/IPCClient.hpp>
#include "session.hpp"
#include "input.hpp"

int main(void){
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
	return 0;
}
