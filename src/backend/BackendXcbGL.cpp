#include <BackendXcbGL.hpp>

BackendXcbGL::BackendXcbGL():
	BackendBase(BackendBase::LibType::OPENGL, BackendBase::SurfType::XCB)
{
/*** Xlib & XCB ***/
	int xlibDefaultScreen;
	xcb_screen_t *xcbScreen = nullptr;
	// Open Xlib display
	xlibDisplay = XOpenDisplay(nullptr);
	if(!xlibDisplay){
		throw "[OpenGL XCB xlib] Can't open X display.";
	}
	xlibDefaultScreen = DefaultScreen(xlibDisplay);
	// Get XCB connection
	xcbConnection = XGetXCBConnection(xlibDisplay);
	if(!xcbConnection){
		throw "[OpenGL XCB connection] Can't get XCB connection.";
	}
	// Set event queue owner
	XSetEventQueueOwner(xlibDisplay, XCBOwnsEventQueue);
	// Get XCB screen
	xcb_screen_iterator_t screenIter = xcb_setup_roots_iterator(xcb_get_setup(xcbConnection));
	for(int screenNum = xlibDefaultScreen; screenIter.rem && screenNum > 0;	--screenNum, xcb_screen_next(&screenIter)){
	}
	xcbScreen = screenIter.data;
/*** GLX ***/
	// Query framebuffer configurations
	GLXFBConfig *fbConfigs = 0;
	int fbConfigsNum = 0;
	fbConfigs = glXGetFBConfigs(xlibDisplay, xlibDefaultScreen, &fbConfigsNum);
	if(!fbConfigs || fbConfigsNum == 0){
		throw "[OpenGL XCB glx] Can't get framebuffer configurations.";
	}
}
BackendXcbGL::~BackendXcbGL(){
	XCloseDisplay(xlibDisplay);
}
void BackendXcbGL::run(){

}