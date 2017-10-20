#ifndef DEAR_SESSION_DEF
#define DEAR_SESSION_DEF

#include <message/IPCClient.hpp>
#include <message/message.hpp>

class Session : public MessageHandler{
public:
	Session();
private:
	DeliverType deliverType;
	IPCClient *ipcClient = nullptr;
	void handleMessage(Message *message, void *deliver, DeliverType type, void *data = nullptr);
	void messageReady(void *deliver, DeliverType type);
};

#endif