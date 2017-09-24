#ifndef BACKENDXCBVK_DEF
#define BACKENDXCBVK_DEF

#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <vulkan/vulkan.hpp>
extern "C"{
#include <xcb/xcb.h>
#include <unistd.h>
}

#include <BackendBase.hpp>

class BackendXcbVK : public BackendBase{
public:
	BackendXcbVK();
	~BackendXcbVK();
	void paint();
private:
	xcb_connection_t *xcbConnection = nullptr;
	xcb_window_t xcbWindow;
};

#endif