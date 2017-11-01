#ifndef DEAR_AREA_MODULE_STARTMENU_DEF
#define DEAR_AREA_MODULE_STARTMENU_DEF

#include <vulkan/vulkan.hpp>
#include <vk/GrVkBackendContext.h>

#include <area/user.hpp>
#include <module/userModule.hpp>


class StartMenuModule : public UserModule {
public: 
	StartMenuModule(User *user);
	void removeModule();
private: 
	User *user;
};

#endif