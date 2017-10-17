#ifndef DEAR_AREA_DEF
#define DEAR_AREA_DEF

#include <message/message.hpp>
#include <message/IPCServer.hpp>
#include <message/TCPServer.hpp>
#include <inputCode.hpp>

class Area : public MessageHandler{
public:
	
private:
	void handleMessage(Message *message, void *deliver, DeliverType type = DEAR_MESSAGE_Client, void *data = nullptr);
	void sendMessage(Message *message, void *deliver, DeliverType type, void *data, uv_write_cb callback = nullptr, void *callbackData = nullptr);
};

#endif