#include "user.hpp"

User::User(IPCServer *server){
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