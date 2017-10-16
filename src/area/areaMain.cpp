extern "C"{
	#include <uv.h>
	#include <sys/stat.h>
	#include <sys/types.h>
	#include <errno.h>
}

#include <message/IPCServer.hpp>
#include "area.hpp"

int main(void){
	// Create IPC directory
	mkdir("/tmp/dear", 0775);
	switch(errno){
		case 0:
		case EEXIST:
		break;
		default:
			throw "[Dear area] Can't create IPC file.";
	}

	Area area;
	uv_loop_t loop;
	uv_loop_init(&loop);
	IPCServer server(&loop, "/tmp/dear/dear-area", &area);
	server.start();
	uv_run(&loop, UV_RUN_DEFAULT);
	uv_loop_close(&loop);
	return 0;
}