#ifndef BACKEND_WAYLAND_DEF
#define BACKEND_WAYLAND_DEF

#include <Base.hpp>

class BackendWayland : public BackendBase {
public:
	BackendWayland(BackendBase::LibType lib = BackendBase::LibType::AUTO_LIB);
private:
	// run()
};

#endif