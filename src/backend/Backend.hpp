#ifndef BACKEND_DEF
#define BACKEND_DEF

#include <BackendGL.hpp>

#ifdef USE_VULKAN
#include <BackendVK.hpp>
#endif

namespace Backend{
	BackendBase *getBackend(BackendBase::LibType lib);
};

#endif