#ifndef DEAR_USER_DEF
#define DEAR_USER_DEF

#include <list>
#include <glm/glm.hpp>

#include <message/IPCServer.hpp>
#include "displayXcb.hpp"

class User{
public:
	User(IPCServer *server);
	Display *display = nullptr;
	glm::vec3 cameraRotation;
	glm::vec3 cameraTranslation;
};
#endif