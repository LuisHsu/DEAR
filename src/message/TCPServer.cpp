#include "TCPServer.hpp"

TCPServer::TCPServer(uv_loop_t *loop, const char *name, const char *service, MessageHandler *handler):
	uvLoop(loop), handler(handler)
{
	// Create pipe
	uv_tcp_init(uvLoop, &serverSocket);
	uv_tcp_nodelay(&serverSocket, true);
	// Address
	struct addrinfo hints;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if(getaddrinfo(name, service, &hints, &serverAddr)){
		throw "[TCPServer] Can't get address info.";
	}
	// Set this as data
	serverSocket.data = this;
	// Bind
	if(uv_tcp_bind(&serverSocket, (sockaddr *)serverAddr, 0)){
		throw "[TCPServer] Can't bind socket.";
	}
}
TCPServer::~TCPServer(){
}
void TCPServer::start(){
	// Listen
	uv_listen((uv_stream_t*)&serverSocket, 10, [](uv_stream_t* server, int status){
		// Check status
		if(status < 0){
			throw "[TCPServer] Can't listen server.";
		}
		// Get server
		TCPServer *tcpServer = (TCPServer *)server->data;
		uv_stream_t *clientSock = nullptr;
		// Accept
		if(uv_accept(server, clientSock) >= 0){
			// Push client stream
			tcpServer->clientStreams.push_back(clientSock);
			// Set data
			clientSock->data = tcpServer;
			// Start read
			uv_read_start(clientSock,
			// Alloc callback
			[](uv_handle_t* , size_t suggested_size, uv_buf_t* buf){
				buf->base = (char *)malloc(suggested_size);
				buf->len = suggested_size;
			},
			// Read callback
			[](uv_stream_t* clientStream, ssize_t len, const uv_buf_t* buf){
				// Get server
				TCPServer *tcpServer = (TCPServer *)clientStream->data;
				std::vector<char> &messageBuf = tcpServer->messageBuf;
				// Handing error
				if(len < 0){
					std::cerr << "[TCPServer] Error receiving from client." << std::endl;
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
					tcpServer->handler->handleMessage((Message *)cur, tcpServer);
					int msgLen = ((Message *)cur)->length;
					cur += msgLen;
					remain -= msgLen;
				}
				// Erase data
				if(cur != messageBuf.data()){
					messageBuf.erase(messageBuf.begin(), messageBuf.begin() + (cur - messageBuf.data()));
				}
			});
		}
	});
}