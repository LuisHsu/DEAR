#ifndef BACKENDDISPLAYGL_DEF
#define BACKENDDISPLAYGL_DEF

extern "C"{
	#include <fcntl.h>
	#include <xf86drm.h>
	#include <xf86drmMode.h>
}

#include <Base.hpp>

class BackendDisplayGL : public BackendBase{
public:
	BackendDisplayGL();
	void run();

private:
	int drmFd;
	drmModeRes *resources = nullptr;
	drmModeConnector *connector = nullptr;
};

#endif