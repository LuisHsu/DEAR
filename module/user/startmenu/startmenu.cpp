#include "startmenu.hpp"

StartMenuModule::StartMenuModule(User *user):user(user){
	Display *display = user->display;
/*** Skia ***/
	
}

void StartMenuModule::removeModule(){
	delete this;
}

UserModule *createModule(void *user){
	return new StartMenuModule((User *)user);
}