#ifndef MESSAGE_DEF
#define MESSAGE_DEF

#include <request.hpp>

class MessageHandler{
public:
	virtual void handleMessage(Message *message, void *server) = 0;
};

#endif