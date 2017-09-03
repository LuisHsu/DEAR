#include <iostream>
#include <backend/Backend.hpp>

int main(int argc, char *argv[]){
	
	// Init backend
	BackendBase *backend = Backend::getBackend(BackendBase::LibType::AUTO_LIB, BackendBase::WMType::AUTO_WM);

	return 0;
}