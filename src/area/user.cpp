#include "user.hpp"

User::User(IPCServer *server):
	cameraRotation(glm::vec3(0.0f, 0.0f, 0.0f)),
	cameraDirection(glm::vec3(0.0f, 0.0f, -1.0f)),
	cameraUp(glm::vec3(0.0f, 1.0f, 0.0f)),
	cameraPosition(glm::vec3(0.0f, 0.0f, 0.0f)),
	cameraFOV(60.0f)
{
	// Get display
	try{
		display = new DisplayXcb(server);
	}catch(std::exception e){
		std::cerr << "XCB display init error: " << e.what() << ". Try direct display." << std::endl;
		display = nullptr;
	}catch(const char *e){
		std::cerr << "XCB display init error: " << e << ". Try direct display." << std::endl;
		display = nullptr;
	}
	// Init
	if(display){
		display->localInit();
	}
}
void User::addHandler(ControlHandler *handler){
	controlHandlers.push_back(handler);
}
void User::popHandler(){
	controlHandlers.pop_back();
}
ControlHandler* User::currentHandler(){
	return controlHandlers.back();
}