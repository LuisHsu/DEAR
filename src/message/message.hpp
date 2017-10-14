#ifndef MESSAGE_DEF
#define MESSAGE_DEF

#include <message/request.hpp>
#include <message/reply.hpp>

class MessageHandler{
public:
	virtual void handleMessage(Message *message, void *server) = 0;
};

#endif