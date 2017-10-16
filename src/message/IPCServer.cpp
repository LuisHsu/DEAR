#include "IPCServer.hpp"

IPCServer::IPCServer(uv_loop_t *loop, const char *path, MessageHandler *handler):
	uvLoop(loop), handler(handler), connecting(false)
{
	// Create pipe
	uv_pipe_init(uvLoop, &serverPipe, 1);
	clientPipe = new uv_pipe_t;
	uv_pipe_init(uvLoop, clientPipe, 1);
	// Set this as data
	serverPipe.data = this;
	// Unlink former
	unlink(path);
	// Bind
	if(uv_pipe_bind(&serverPipe, path)){
		throw "[IPCServer] Can't bind pipe.";
	}
}
IPCServer::~IPCServer(){
	uv_read_stop((uv_stream_t*)&serverPipe);
}
void IPCServer::start(){
	// Listen
	uv_listen((uv_stream_t*)&serverPipe, 1, [](uv_stream_t* server, int status){
		// Check status
		if(status < 0){
			throw "[IPCServer] Can't listen server.";
		}
		// Get data
		IPCServer *ipcServer = (IPCServer *)server->data;
		// Accept
		if(!(ipcServer->connecting) && (uv_accept(server, (uv_stream_t*)(ipcServer->clientPipe)) >= 0)){
			std::cerr << "[IPCServer] Client connected." << std::endl;
			// Set connecting
			ipcServer->connecting = true;
			// Set data
			ipcServer->clientPipe->data = ipcServer;
			// Start read
			uv_read_start((uv_stream_t*)(ipcServer->clientPipe),
			// Alloc callback
			[](uv_handle_t* , size_t suggested_size, uv_buf_t* buf){
				buf->base = (char *)malloc(suggested_size);
				buf->len = suggested_size;
			},
			// Read callback
			[](uv_stream_t* clientPipe, ssize_t len, const uv_buf_t* buf){
				IPCServer *ipcServer = (IPCServer *)clientPipe->data;
				std::vector<char> &messageBuf = ipcServer->messageBuf;
				// Handing error
				if(len < 0){
					std::cerr << "[IPCServer] Client disconnected." << std::endl;
					uv_close((uv_handle_t*)(ipcServer->clientPipe), [](uv_handle_t* handle){
						std::cerr << "[IPCServer] Client closed." << std::endl;
						IPCServer *ipcServer = (IPCServer *)handle->data;
						ipcServer->messageBuf.clear();
						// Set connecting
						ipcServer->connecting = false;
						delete ipcServer->clientPipe;
						ipcServer->clientPipe = new uv_pipe_t;
						uv_pipe_init(ipcServer->uvLoop, ipcServer->clientPipe, 1);
					});
					return;
				}
				// Append buf
				messageBuf.resize(messageBuf.size() + len);
				// Copy data
				memcpy(messageBuf.data() + messageBuf.size() - len, buf->base, len);
				// Free buffer
				delete [] buf->base;
				// Dealing with message
				char *cur = messageBuf.data();
				for(uint32_t remain = messageBuf.size(); remain >= sizeof(Message) && remain >= ((Message *)cur)->length;){
					ipcServer->handler->handleMessage((Message *)cur, ipcServer);
					int msgLen = ((Message *)cur)->length;
					cur += msgLen;
					remain -= msgLen;
				}
				if(cur != messageBuf.data()){
					messageBuf.erase(messageBuf.begin(), messageBuf.begin() + (cur - messageBuf.data()));
				}
			});
		}
	});
}
void IPCServer::sendMessage(Message *message, uv_write_cb callback , void *callbackData){
	uv_write_t *req = new uv_write_t;
	req->data = callbackData;
	uv_buf_t buf = {
		.base = (char *)message,
		.len = message->length
	};
	uv_write(req, (uv_stream_t *)&clientPipe, &buf, 1, callback);
}