#ifndef DEAR_MESSAGE_DEF
#define DEAR_MESSAGE_DEF

#include "request.hpp"
#include "notice.hpp"

class MessageHandler{
public:
	virtual void handleMessage(Message *message, void *server) = 0;
};

#endif