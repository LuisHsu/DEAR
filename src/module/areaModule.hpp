#ifndef DEAR_AREA_MODULE_DEF
#define DEAR_AREA_MODULE_DEF

#include <area/controlHandler.hpp>

class AreaModule : public ControlHandler{
public: 
	virtual ~AreaModule(){};
	virtual void userInit(void *user){};
	virtual void removeModule() = 0;
};

typedef AreaModule *(*area_module_create_t)(void *area);

extern "C" AreaModule *createModule(void *area);

#endif