#ifndef DEAR_REQUEST_DEF
#define DEAR_REQUEST_DEF

#include <message/messageBase.hpp>

class KeyboardRequest : public Message{
public: 
	uint32_t time;
	uint64_t utime;
	uint32_t key;
	uint32_t count;
};

#endif