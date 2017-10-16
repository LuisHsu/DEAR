#ifndef DEAR_MESSAGEBASE_DEF
#define DEAR_MESSAGEBASE_DEF

#include <cstdint>

enum MessageType{
	IPC_Connect_request,
	IPC_Connect_notice,
	KeyDown_request,
	KeyUp_request
};

class Message{
public:
	MessageType type;
	uint32_t length;
};

#endif