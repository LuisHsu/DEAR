#include <IPCClient.hpp>

IPCClient::IPCClient(uv_loop_t *loop, MessageHandler *handler):
	uvLoop(loop), handler(handler)
{
	// Create pipe
	uv_pipe_init(uvLoop, &clientPipe, 1);
	// Set this as data
	clientPipe.data = this;
	uvConnect.data = this;
}
IPCClient::~IPCClient(){
	uv_read_stop((uv_stream_t*)&clientPipe);
}
void IPCClient::start(const char *path){
	// Listen
	uv_pipe_connect(&uvConnect, &clientPipe, path, [](uv_connect_t* connect, int status){
		// Get data
		IPCClient *ipcClient = (IPCClient *)connect->data;
		if(status < 0){
			throw "[IPCClient] Can't connect to server.";
		}
		// Start read
		uv_read_start((uv_stream_t*)&(ipcClient->clientPipe),
		// Alloc callback
		[](uv_handle_t* , size_t suggested_size, uv_buf_t* buf){
			buf->base = (char *)malloc(suggested_size);
			buf->len = suggested_size;
		},
		// Read callback
		[](uv_stream_t* client, ssize_t len, const uv_buf_t* buf){
			IPCClient *ipcClient = (IPCClient *)client->data;
			std::vector<char> &messageBuf = ipcClient->messageBuf;
			// Handing error
			if(len < 0){
				std::cerr << "[IPCClient] Server disconnected." << std::endl;
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
				ipcClient->handler->handleMessage((Message *)cur, ipcClient);
				int msgLen = ((Message *)cur)->length;
				cur += msgLen;
				remain -= msgLen;
			}
			// Erase data
			if(cur != messageBuf.data()){
				messageBuf.erase(messageBuf.begin(), messageBuf.begin() + (cur - messageBuf.data()));
			}
		});
	});
}
void IPCClient::sendMessage(Message *message, uv_write_cb callback , void *callbackData){
	uv_write_t *req = new uv_write_t;
	req->data = callbackData;
	uv_buf_t buf = {
		.base = (char *)message,
		.len = message->length
	};
	uv_write(req, (uv_stream_t *)&clientPipe, &buf, 1, callback);
}
