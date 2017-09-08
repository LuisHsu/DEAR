#include <iostream>
#include <backend/Backend.hpp>
#include <Config.hpp>

int main(int argc, char *argv[]){
	try{
		// Read config
		Config::readconf();

		// Init backend
		Backend *backend = new Backend(Backend::LibType::VULKAN);
		
		// Clean
		delete backend;
	}catch(const char *e){
		std::cerr << e << std::endl;
	}
	return 0;
}