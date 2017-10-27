#ifndef DEAR_USER_DEF
#define DEAR_USER_DEF

#include <deque>
#include <glm/glm.hpp>

#include <message/IPCServer.hpp>
#include "displayXcb.hpp"
#include "controlHandler.hpp"

class User{
public:
	User(IPCServer *server);
	void addHandler(ControlHandler *handler);
	void popHandler();
	Display *display = nullptr;
	glm::vec3 cameraRotation;
	glm::vec3 cameraTranslation;
private:
	std::deque<ControlHandler *> controlHandlers;
};
#endif