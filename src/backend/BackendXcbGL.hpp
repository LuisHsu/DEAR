#ifndef BACKENDXCBGL_DEF
#define BACKENDXCBGL_DEF

extern "C"{
	#include <X11/Xlib.h>
	#include <X11/Xlib-xcb.h>
	#include <xcb/xcb.h>
    #include <GL/glx.h>
    #include <GL/gl.h>
}

#include <cstring>
#include <Base.hpp>

class BackendXcbGL : public BackendBase{
public:
	BackendXcbGL();
	~BackendXcbGL();
	void paint();

private:
	Display *xlibDisplay;
	xcb_connection_t *xcbConnection = nullptr;
	xcb_window_t xcbWindow;
	GLXFBConfig glxFbConfig;
	GLXContext glxContext;
	GLXWindow glxWindow;
};

#endif