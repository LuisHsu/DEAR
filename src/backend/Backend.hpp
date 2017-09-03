#ifndef BACKEND_DEF
#define BACKEND_DEF

#include <iostream>
#include <cstring>
#include <vector>

#ifdef USE_VULKAN
#include <vulkan/vulkan.h>
#endif

#include <Base.hpp>
#include <Wayland.hpp>
#include <Xcb.hpp>
#include <Display.hpp>

namespace Backend{
	BackendBase* getBackend(BackendBase::LibType lib, BackendBase::WMType wm);
	#ifdef USE_VULKAN
	void initVK(BackendBase* &backend, BackendBase::LibType &lib, BackendBase::WMType &wm);
	#endif
	//void initGL();
}

#endif