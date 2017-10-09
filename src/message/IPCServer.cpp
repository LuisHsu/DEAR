#include <IPCServer.hpp>

IPCServer::IPCServer(uv_loop_t *loop, const char *path){
	// Create pipe
	uv_pipe_init(loop, &uvPipe, 1);
	// Bind
	if(uv_pipe_bind(&uvPipe, path){
		throw "[IPCServer] Can't bind pipe.";
	}
}
IPCServer::~IPCServer(){
	uv_close(&uvPipe, nullptr);
}
void IPCServer::start(MessageHandler *handler){
	// Listen
	uv_listen(&uvPipe, 1, [&](uv_stream_t* server, int status){
		if(status < 0){
			throw "[IPCServer] Can't listen server.";
		}
		// Accept
		if(uv_accept(server, clientSock) >= 0){
			// Start read
			uv_read_start(&uvPipe,
			// Alloc callback
			[](uv_handle_t* , size_t suggested_size, uv_buf_t* buf){
				buf->base = malloc(suggested_size);
				buf->len = suggested_size;
			},
			// Read callback
			[&](uv_stream_t* uvPipe, ssize_t len, const uv_buf_t* buf){
				// Handing error
				if(len < 0){
					std::cerr << "[IPCServer] Error receiving from session." << std::endl;
					return;
				}
				// Append buf
				messageBuf.resize(messageBuf.size() + len);
				// Copy data
				memcpy(messageBuf.data() + messageBuf.size() - len, buf->base, len);
				// Dealing with message
				char *cur = messageBuf.data();
				for(uint32_t remain = messageBuf.size(); remain >= sizeof(Message) && remain >= ((Message *)cur)->length;){
					handler->handleMessage((Message *)cur);
					int msgLen = ((Message *)cur)->length;
					cur += msgLen;
					remain -= msgLen;
				}
				messageBuf.erase(messageBuf.begin(), messageBuf.begin() + (cur - messageBuf.data()));
			});
		}
	});
}