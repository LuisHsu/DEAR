#include "startmenu.hpp"

StartMenuModule::StartMenuModule(User *user):user(user){
	Display *display = user->display;
/*** Skia init ***/
	GrVkBackendContext* vkBackendCtxSkia = display->createSkiaContext();
}

void StartMenuModule::removeModule(){
	delete this;
}

UserModule *createModule(void *user){
	return new StartMenuModule((User *)user);
}