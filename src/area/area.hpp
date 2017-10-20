#ifndef DEAR_AREA_DEF
#define DEAR_AREA_DEF

extern "C"{
	#include <uv.h>
}

#include <vector>

#include <message/message.hpp>
#include <message/IPCServer.hpp>
#include <message/TCPServer.hpp>
#include <inputCode.hpp>
#include "displayXcb.hpp"
#include "user.hpp"

class Area : public MessageHandler{
public:
	
private:
	Display *display;
	std::vector<User> users;
	Display *getDisplay(IPCServer *server);
	void handleMessage(Message *message, void *deliver, DeliverType type, void *data = nullptr);
	void sendMessage(Message *message, void *deliver, DeliverType type, void *data, uv_write_cb callback = nullptr, void *callbackData = nullptr);
	void ipcConnect(Message *message, void *deliver, DeliverType type, void *data);
	void messageReady(void *server, DeliverType type);
	void pointerMotion(Message *message, void *deliver, DeliverType type, void *data);
};

#endif