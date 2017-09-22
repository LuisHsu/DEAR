#include <BackendXcbGL.hpp>

BackendXcbGL::BackendXcbGL():
	BackendBase(BackendBase::LibType::OPENGL, BackendBase::SurfType::XCB)
{
	int xlibDefaultScreen;
	xcb_screen_t *xcbScreen = nullptr;
	xcb_colormap_t xcbColormap;
/*** Xlib, XCB & GLX ***/
	// Open Xlib display
	xlibDisplay = XOpenDisplay(nullptr);
	if(!xlibDisplay){
		throw "[OpenGL XCB xlib] Can't open X display.";
	}
	xlibDefaultScreen = DefaultScreen(xlibDisplay);
	// Get XCB connection
	xcbConnection = XGetXCBConnection(xlibDisplay);
	if(!xcbConnection){
		XCloseDisplay(xlibDisplay);
		throw "[OpenGL XCB connection] Can't get XCB connection.";
	}
	// Set event queue owner
	XSetEventQueueOwner(xlibDisplay, XCBOwnsEventQueue);
	// Get XCB screen
	xcb_screen_iterator_t screenIter = xcb_setup_roots_iterator(xcb_get_setup(xcbConnection));
	for(int screenNum = xlibDefaultScreen; screenIter.rem && screenNum > 0;	--screenNum, xcb_screen_next(&screenIter)){
	}
	xcbScreen = screenIter.data;
	// Query framebuffer configurations
	GLXFBConfig *fbConfigs = 0;
	int fbConfigsNum = 0;
	fbConfigs = glXGetFBConfigs(xlibDisplay, xlibDefaultScreen, &fbConfigsNum);
	if(!fbConfigs || fbConfigsNum == 0){
		XCloseDisplay(xlibDisplay);
		throw "[OpenGL XCB glx] Can't get framebuffer configurations.";
	}
	glxFbConfig = fbConfigs[0];
	// glx Context
	int visualID = 0;	
	glXGetFBConfigAttrib(xlibDisplay, glxFbConfig, GLX_VISUAL_ID , &visualID);
	glxContext = glXCreateNewContext(xlibDisplay, glxFbConfig, GLX_RGBA_TYPE, 0, True);
	if(!glxContext){
		XCloseDisplay(xlibDisplay);
		throw "[OpenGL XCB glx] Can't get framebuffer configurations.";
	}
	// XCB colormap & window
	xcbColormap = xcb_generate_id(xcbConnection);
	xcbWindow = xcb_generate_id(xcbConnection);
	xcb_create_colormap(
		xcbConnection,
		XCB_COLORMAP_ALLOC_NONE,
		xcbColormap,
		xcbScreen->root,
		visualID
	);
	uint32_t eventmask = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS;
	uint32_t valuelist[] = { eventmask, xcbColormap, 0 };
	uint32_t valuemask = XCB_CW_EVENT_MASK | XCB_CW_COLORMAP;
	xcb_create_window(
		xcbConnection,
		XCB_COPY_FROM_PARENT,
		xcbWindow,
		xcbScreen->root,
		0, 0,
		xcbScreen->width_in_pixels, xcbScreen->height_in_pixels,
		0,
		XCB_WINDOW_CLASS_INPUT_OUTPUT,
		visualID,
		valuemask,
		valuelist
	);
	// Set window property
	const char *windowTitle = "DEAR Desktop";
	xcb_change_property (xcbConnection,
		XCB_PROP_MODE_REPLACE,
		xcbWindow,
		XCB_ATOM_WM_NAME,
		XCB_ATOM_STRING,
		8,
		strlen(windowTitle),
		windowTitle
	);
	// Map window
	xcb_map_window(xcbConnection, xcbWindow);
	// GLX window
	glxWindow = glXCreateWindow(xlibDisplay, glxFbConfig, xcbWindow, nullptr);
	if(!glXMakeContextCurrent(xlibDisplay, glxWindow, glxWindow, glxContext)){
		glXDestroyWindow(xlibDisplay, glxWindow);
		xcb_destroy_window(xcbConnection, xcbWindow);
		glXDestroyContext(xlibDisplay, glxContext);
		XCloseDisplay(xlibDisplay);
		throw "[OpenGL XCB glx] Can't make GLX content current";
	}
}
BackendXcbGL::~BackendXcbGL(){
	glXDestroyWindow(xlibDisplay, glxWindow);
	xcb_destroy_window(xcbConnection, xcbWindow);
	glXDestroyContext(xlibDisplay, glxContext);
	XCloseDisplay(xlibDisplay);
}
void BackendXcbGL::paint(){
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glXSwapBuffers(xlibDisplay, glxWindow);
}