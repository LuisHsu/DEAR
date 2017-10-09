#include <TCPServer.hpp>

TCPServer::TCPServer(uv_loop_t *loop, const char *path){
	// Create pipe
	uv_tcp_init(loop, &uvSocket);
	uv_tcp_nodelay(&uvSocket, true);
	// Bind
	if(uv_tcp_bind(&uvPipe, path){
		throw "[TCPServer] Can't bind pipe.";
	}
}
TCPServer::~TCPServer(){
	uv_close(&uvSocket, nullptr);
}
void TCPServer::start(MessageHandler *handler){
	// Listen
	uv_listen(&uvSocket, 1, [&](uv_stream_t* server, int status){
		if(status < 0){
			throw "[TCPServer] Can't listen server.";
		}
		// Accept
		if(uv_accept(server, clientSock) >= 0){
			// Start read
			uv_read_start(&uvSocket,
			// Alloc callback
			[](uv_handle_t* , size_t suggested_size, uv_buf_t* buf){
				buf->base = malloc(suggested_size);
				buf->len = suggested_size;
			},
			// Read callback
			[&](uv_stream_t* uvSocket, ssize_t len, const uv_buf_t* buf){
				// Handing error
				if(len < 0){
					std::cerr << "[TCPServer] Error receiving from session." << std::endl;
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