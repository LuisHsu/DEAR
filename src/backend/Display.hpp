#ifndef BACKEND_DISPLAY_DEF
#define BACKEND_DISPLAY_DEF

#include <Base.hpp>

class BackendDisplay : public BackendBase {
public:
	BackendDisplay(BackendBase::LibType lib = BackendBase::LibType::AUTO_LIB);

	#ifdef USE_VULKAN
	void createSurface();
	#endif

private:
	// run()
};

#endif