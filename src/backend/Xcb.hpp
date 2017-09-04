#ifndef BACKEND_XCB_DEF
#define BACKEND_XCB_DEF

#include <Base.hpp>
#include <xcb/xcb.h>

class BackendXcb : public BackendBase {
public:
	BackendXcb(BackendBase::LibType lib = BackendBase::LibType::AUTO_LIB);
	xcb_connection_t *connection;
	xcb_screen_t *screen;
	uint32_t windowId;
private:
	// run()
};

#endif