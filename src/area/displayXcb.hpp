#ifndef DEAR_DISPLAYXCB_DEF
#define DEAR_DISPLAYXCB_DEF

extern "C"{
	#include <xcb/xcb.h>
}
#include <cstring>

#include "display.hpp"

class DisplayXcb : public Display{
public:
	DisplayXcb();
private:
	xcb_connection_t *connectionXcb = nullptr;
	xcb_window_t windowXcb;
};

#endif