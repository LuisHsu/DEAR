#ifndef DEAR_SESSION_DEF
#define DEAR_SESSION_DEF

#include <message/IPCClient.hpp>
#include <message/message.hpp>

class Session : public MessageHandler{
public:

private:
	void handleMessage(Message *message, void *deliver, DeliverType type = DEAR_MESSAGE_Client, void *data = nullptr);
};

#endif