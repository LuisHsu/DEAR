#ifndef DEAR_MESSAGE_DEF
#define DEAR_MESSAGE_DEF

#include "request.hpp"
#include "reply.hpp"

class MessageHandler{
public:
	virtual void handleMessage(Message *message, void *server) = 0;
};

#endif