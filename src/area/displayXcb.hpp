#ifndef DEAR_DISPLAYXCB_DEF
#define DEAR_DISPLAYXCB_DEF

extern "C"{
	#include <xcb/xcb.h>
	#include <uv.h>
}
#include <cstring>
#include <iostream>

#include <message/IPCServer.hpp>
#include "display.hpp"

class DisplayXcb : public Display{
public:
	DisplayXcb(IPCServer* server);
private:
	IPCServer *server;
	uv_poll_t xcbPollUv;
	xcb_connection_t *connectionXcb = nullptr;
	xcb_window_t windowXcb;
	xcb_intern_atom_reply_t *closeReply = nullptr;
	void handleXcbEvent(xcb_generic_event_t *event);
};

#endif