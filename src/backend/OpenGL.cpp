#include <Backend.hpp>

void Backend::initGL(){
/*** DRM ***/
	if((drmFd = open("/dev/dri/card0", O_RDWR)) < 0){
		throw "[OpenGL drm] Can't open DRI card";
	}
	if(!(resources = drmModeGetResources(drmFd))){
		throw "[OpenGL drm] Get resources failed";
	}
	for (int i = 0; i < resources->count_connectors; ++i) {
		connector = drmModeGetConnector(drm.fd, resources->connectors[i]);
		if (connector->connection == DRM_MODE_CONNECTED) {
			/* it's connected, let's use this! */
			break;
		}
		drmModeFreeConnector(connector);
		connector = NULL;
	}
}