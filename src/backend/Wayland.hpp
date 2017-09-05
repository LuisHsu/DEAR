#ifndef BACKEND_WAYLAND_DEF
#define BACKEND_WAYLAND_DEF

#include <Base.hpp>
#include <Config.hpp>

#include <wayland-client.h>

class BackendWayland : public BackendBase {
public:
	BackendWayland(BackendBase::LibType lib = BackendBase::LibType::AUTO_LIB);
	~BackendWayland();

	#ifdef USE_VULKAN
	void createSurface();
	#endif

	wl_display *display;

private:
	// run()
};

#endif