#include <Wayland.hpp>

BackendWayland::BackendWayland(BackendBase::LibType lib):
	BackendBase(lib)
{
	display = wl_display_connect(nullptr);

}

BackendWayland::~BackendWayland(){
	wl_display_disconnect(display);
}

void BackendWayland::createSurface(){
	
}