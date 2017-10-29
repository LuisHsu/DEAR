#ifndef DEAR_CONTROLHANDLER_DEF
#define DEAR_CONTROLHANDLER_DEF

#include <message/message.hpp>

class ControlHandler{
public:
	virtual void pointerMotion(Message *message, void *deliver, MessageHandler::DeliverType type, void *data){};
	virtual void pointerAxis(Message *message, void *deliver, MessageHandler::DeliverType type, void *data){};
};

#endif