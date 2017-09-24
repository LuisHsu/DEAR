#include <iostream>
#include <Backend.hpp>
#include <AreaNode.hpp>
#include <Config.hpp>

int main(int argc, char *argv[]){
	try{
		// Read config
		Config::readconf();
		// Init backend
		BackendBase *backend = Backend::getBackend();
		// Create greeter node
		AreaNode greeter("/tmp/dear_greeter", backend);
		// Clean
		delete backend;
	}catch(const char *e){
		std::cerr << e << std::endl;
	}
	return 0;
}
