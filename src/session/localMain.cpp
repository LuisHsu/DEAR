extern "C"{
	#include <uv.h>
}
#include <iostream>
#include <message/IPCClient.hpp>
#include <session.hpp>

int main(void){
	Session session;
	uv_loop_t loop;
	uv_loop_init(&loop);
	IPCClient client(&loop, &session);
	client.start("/tmp/dear/dear-area");
	Message testMessage = {.type = IPC_Connect_req};
	testMessage.length = sizeof(testMessage);
	client.sendMessage(&testMessage,
	[](uv_write_t* req, int status){
		std::cout << status << std::endl;
		delete req;
	},&testMessage);
	uv_run(&loop, UV_RUN_DEFAULT);
	uv_loop_close(&loop);
	return 0;
}
