#ifndef DEAR_MESSAGE_DEF
#define DEAR_MESSAGE_DEF

#include "request.hpp"
#include "notice.hpp"

class MessageHandler{
public:
	enum DeliverType{
		DEAR_MESSAGE_IPCClient,
		DEAR_MESSAGE_TCPClient,
		DEAR_MESSAGE_IPCserver,
		DEAR_MESSAGE_TCPserver
	};
	virtual void handleMessage(Message *message, void *deliver, DeliverType type, void *data = nullptr) = 0;
	virtual void messageReady(void *deliver, DeliverType type) = 0;
};

#endif