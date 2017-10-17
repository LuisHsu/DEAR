#ifndef DEAR_MESSAGE_DEF
#define DEAR_MESSAGE_DEF

#include "request.hpp"
#include "notice.hpp"

class MessageHandler{
public:
	enum DeliverType{
		DEAR_MESSAGE_Client,
		DEAR_MESSAGE_IPCserver,
		DEAR_MESSAGE_TCPserver
	};
	virtual void handleMessage(Message *message, void *deliver, DeliverType type = DEAR_MESSAGE_Client, void *data = nullptr) = 0;
};

#endif