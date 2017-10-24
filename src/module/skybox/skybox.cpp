#include "skybox.hpp"

SkyBoxModule::SkyBoxModule(Area *area):area(area){
	
}

void SkyBoxModule::removeModule(){
	delete this;
}

AreaModule *createModule(void *area){
	return new SkyBoxModule((Area *)area);
}