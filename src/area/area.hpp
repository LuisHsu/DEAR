#ifndef DEAR_AREA_DEF
#define DEAR_AREA_DEF

#include <message/message.hpp>

class Area : public MessageHandler{
public:
	
private:
	void handleMessage(Message *message, void *server);
};

#endif