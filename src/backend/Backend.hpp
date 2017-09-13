#ifndef BACKEND_DEF
#define BACKEND_DEF

#include <BackendGL.hpp>

#ifdef USE_VULKAN
#include <BackendDisplayVK.hpp>
#ifdef XCB_SUPPORT
#include <BackendXcbVK.hpp>
#endif
#endif

namespace Backend{
	BackendBase *getBackend(BackendBase::LibType lib);
};

#endif