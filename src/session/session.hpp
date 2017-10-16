#ifndef DEAR_SESSION_DEF
#define DEAR_SESSION_DEF

#include <message/message.hpp>

class Session : public MessageHandler{
public:

private:
	void handleMessage(Message *message, void *server);
};

#endif