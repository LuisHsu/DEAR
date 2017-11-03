#ifndef DEAR_AREA_MODULE_STARTMENU_DEF
#define DEAR_AREA_MODULE_STARTMENU_DEF

#include <area/user.hpp>
#include <module/userModule.hpp>

class StartMenuModule : public UserModule {
public: 
	StartMenuModule(User *user);
	void removeModule();
private: 
	User *user;
	sk_sp<GrContext> skiaCtx;
	VkImage imageVk;
	VkDeviceMemory imageMemoryVk;
	sk_sp<SkSurface> surfaceSkia;
	GrBackendTexture backendTextureSkia;
};

#endif