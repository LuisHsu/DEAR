#include <BackendGL.hpp>

BackendGL::BackendGL():
	BackendBase(BackendBase::LibType::OPENGL)
{
/*** DRM ***/
	if((drmFd = open("/dev/dri/card0", O_RDWR)) < 0){
		throw "[OpenGL drm] Can't open DRI card";
	}
	resources = drmModeGetResources(drmFd);
	if(!resources){
		throw "[OpenGL drm] Get resources failed";
	}
	for (int i = 0; i < resources->count_connectors; ++i) {
		connector = drmModeGetConnector(drmFd, resources->connectors[i]);
		if (connector->connection == DRM_MODE_CONNECTED) {
			break;
		}
		drmModeFreeConnector(connector);
		connector = nullptr;
	}
}
void BackendGL::run(){

}