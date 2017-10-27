#ifndef DEAR_MESSAGEBASE_DEF
#define DEAR_MESSAGEBASE_DEF

#include <cstdint>

enum MessageType{
	DEAR_Connect_request,
	DEAR_Connect_notice,
	DEAR_KeyDown_request,
	DEAR_KeyUp_request,
	DEAR_PointerMotion_request,
	DEAR_PointerUp_request,
	DEAR_PointerDown_request,
	DEAR_PointerAxis_request,
	DEAR_Close_notice
};

class Message{
public:
	MessageType type;
	uint32_t length;
};

#endif