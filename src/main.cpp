#include <iostream>
#include <backend/Backend.hpp>
#include <Config.hpp>

int main(int argc, char *argv[]){
	// Read config
	Config::readconf();

	// Init backend
	BackendBase *backend = Backend::getBackend(BackendBase::LibType::AUTO_LIB, BackendBase::WMType::AUTO_WM);

	// Clean
	delete backend;
	return 0;
}