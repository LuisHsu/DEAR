#ifndef DEAR_USER_MODULE_DEF
#define DEAR_USER_MODULE_DEF

#include <area/controlHandler.hpp>

class UserModule : public ControlHandler{
public: 
	virtual ~UserModule(){};
	virtual void removeModule() = 0;
};

typedef UserModule *(*user_module_create_t)(void *user);

extern "C" UserModule *createModule(void *user);

#endif