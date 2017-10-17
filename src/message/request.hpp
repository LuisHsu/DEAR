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

class PointerMotionRequest : public Message{
public: 
	uint32_t time;
	uint64_t utime;
	double dx;
	double dy;
};

class PointerButtonRequest : public Message{
public: 
	uint32_t time;
	uint64_t utime;
	uint32_t button;
	uint32_t count;
};

class PointerAxisRequest : public Message{
public: 
	enum AxisSource{
		DEAR_POINTER_AXIS_SRC_Wheel,
		DEAR_POINTER_AXIS_SRC_Finger,
		DEAR_POINTER_AXIS_SRC_Continuous
	};
	enum Axis{
		DEAR_POINTER_AXIS_Horizontal = 1,
		DEAR_POINTER_AXIS_Vertical = 2,
	};
	uint32_t time;
	uint64_t utime;
	AxisSource source;
	uint32_t axis;
	double valueH;
	double valueV;
};

#endif