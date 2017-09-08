#ifndef BACKENDGL_DEF
#define BACKENDGL_DEF

extern "C"{
	#include <fcntl.h>
	#include <xf86drm.h>
	#include <xf86drmMode.h>
}

#include <Base.hpp>

class BackendGL : public BackendBase{
public:
	BackendGL();

private:
	int drmFd;
	drmModeRes *resources = nullptr;
	drmModeConnector *connector = nullptr;
};

#endif