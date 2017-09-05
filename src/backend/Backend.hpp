#ifndef BACKEND_DEF
#define BACKEND_DEF

#include <iostream>
#include <cstring>
#include <vector>

#ifdef USE_VULKAN
#include <vulkan/vulkan.hpp>
#endif

#include <Base.hpp>
#include <Display.hpp>

#ifdef WAYLAND_SUPPORT
#include <Wayland.hpp>
#endif
#ifdef XCB_SUPPORT
#include <Xcb.hpp>
#endif

namespace Backend{
	BackendBase* getBackend(BackendBase::LibType lib, BackendBase::WMType wm);

	#ifdef USE_VULKAN
	void initVK(BackendBase* &backend, BackendBase::LibType &lib, BackendBase::WMType &wm);
	VkPhysicalDevice pickPhyDevice(VkInstance instance, BackendBase::WMType &wm);
	#endif

	//void initGL();
}

#endif