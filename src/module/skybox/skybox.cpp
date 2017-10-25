#include "skybox.hpp"

#include <iostream>

SkyBoxModule::SkyBoxModule(Area *area):area(area){
	std::cout << "SkyBox" << std::endl;
}

void SkyBoxModule::removeModule(){
	delete this;
}

AreaModule *createModule(void *area){
	return new SkyBoxModule((Area *)area);
}