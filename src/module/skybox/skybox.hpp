#ifndef DEAR_AREA_MODULE_SKYBOX_DEF
#define DEAR_AREA_MODULE_SKYBOX_DEF

#include <areaModule.hpp>
#include <area/area.hpp>

class SkyBoxModule : public AreaModule {
public: 
	SkyBoxModule(Area *area);
	void removeModule();
private: 
	Area *area;
};

#endif