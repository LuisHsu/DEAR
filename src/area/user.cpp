#include "user.hpp"

User::User(IPCServer *server):
	cameraRotation(glm::vec3(0.0f, 0.0f, 0.0f)),
	cameraDirection(glm::vec3(0.0f, 0.0f, -1.0f)),
	cameraUp(glm::vec3(0.0f, 1.0f, 0.0f)),
	cameraPosition(glm::vec3(0.0f, 0.0f, 0.0f)),
	cameraFOV(60.0f)
{
/*** Display ***/
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
/*** User module ***/
	const std::string modDir("mod/user");
	// Get module directory
	DIR *areaModDir = opendir(modDir.c_str());
	if(!areaModDir){
		perror("[User module] Open module directory error:");
	}
	// Load modules
	struct dirent *entry = nullptr;
	while((entry = readdir(areaModDir)) != nullptr){
		if(entry->d_type == DT_REG){
			// Get dl handle
			void *modHandle = dlopen((modDir + "/" + entry->d_name).c_str(), RTLD_LAZY);
			if(!modHandle){
				std::cerr << "[User module] Error open module " << entry->d_name << " : " << dlerror() << std::endl;
			}
			// Get create function
			user_module_create_t createFunc = (user_module_create_t) dlsym(modHandle, "createUserModule");
			if(!createFunc){
				std::cerr << "[User module] Error open create function : " << dlerror() << std::endl;
			}
			// Create module instance
			userModules[entry->d_name] = createFunc(this);
		}
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