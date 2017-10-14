#ifndef SESSION_DEF
#define SESSION_DEF

#include <message/message.hpp>

class Session : public MessageHandler{
public:

private:
	void handleMessage(Message *message, void *server);
};

#endif