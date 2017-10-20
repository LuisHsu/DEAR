#ifndef DEAR_AREA_DEF
#define DEAR_AREA_DEF

#include <message/message.hpp>
#include <message/IPCServer.hpp>
#include <message/TCPServer.hpp>
#include <inputCode.hpp>
#include "displayXcb.hpp"

class Area : public MessageHandler{
public:
	Area();
private:
	Display *display;
	void handleMessage(Message *message, void *deliver, DeliverType type, void *data = nullptr);
	void sendMessage(Message *message, void *deliver, DeliverType type, void *data, uv_write_cb callback = nullptr, void *callbackData = nullptr);
	void ipcConnect(Message *message, void *deliver, DeliverType type, void *data);
	void messageReady(void *server, DeliverType type);
	Display *getDisplay();
};

#endif