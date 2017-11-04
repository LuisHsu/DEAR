#ifndef DEAR_USER_DEF
#define DEAR_USER_DEF

extern "C"{
	#include <dlfcn.h>
	#include <sys/types.h>
	#include <dirent.h>
	#include <errno.h>
}

#include <deque>
#include <glm/glm.hpp>
#include <map>
#include <cstdio>

#include <message/IPCServer.hpp>
#include <module/userModule.hpp>
#include "displayXcb.hpp"
#include "controlHandler.hpp"

class User{
public:
	User(IPCServer *server);
	void initModule();
	void addHandler(ControlHandler *handler);
	void popHandler();
	ControlHandler *currentHandler();
	Display *display = nullptr;
	glm::vec3 cameraRotation;
	glm::vec3 cameraDirection;
	glm::vec3 cameraUp;
	glm::vec3 cameraPosition;
	float cameraFOV;
	std::map<std::string, UserModule *> userModules;
private:
	std::deque<ControlHandler *> controlHandlers;
};
#endif