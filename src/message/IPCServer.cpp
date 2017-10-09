#include <IPCServer.hpp>

IPCServer::IPCServer(uv_loop_t *loop, const char *path, MessageHandler *handler):
	uvLoop(loop), handler(handler)
{
	// Create pipe
	uv_pipe_init(uvLoop, &serverPipe, 1);
	// Set this as data
	serverPipe.data = this;
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
		if(uv_accept(server, (uv_stream_t*)&(ipcServer->clientPipe)) >= 0){
			// Stop accept
			uv_read_stop((uv_stream_t*)&(ipcServer->serverPipe));
			// Set data
			ipcServer->clientPipe.data = ipcServer;
			// Start read
			uv_read_start((uv_stream_t*)&(ipcServer->clientPipe),
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
					std::cerr << "[IPCServer] Error receiving from client." << std::endl;
					return;
				}
				// Append buf
				messageBuf.resize(messageBuf.size() + len);
				// Copy data
				memcpy(messageBuf.data() + messageBuf.size() - len, buf->base, len);
				// Dealing with message
				char *cur = messageBuf.data();
				for(int32_t remain = messageBuf.size(); remain >= (int32_t)sizeof(Message) && remain >= ((Message *)cur)->length;){
					ipcServer->handler->handleMessage((Message *)cur, ipcServer);
					int msgLen = ((Message *)cur)->length;
					cur += msgLen;
					remain -= msgLen;
				}
				messageBuf.erase(messageBuf.begin(), messageBuf.begin() + (cur - messageBuf.data()));
			});
		}
	});
}