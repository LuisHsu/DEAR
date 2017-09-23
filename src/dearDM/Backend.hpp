#ifndef BACKEND_DEF
#define BACKEND_DEF

#include <BackendDisplayVK.hpp>
#ifdef XCB_SUPPORT
#include <BackendXcbVK.hpp>
#endif

namespace Backend{
	BackendBase *getBackend();
};

#endif