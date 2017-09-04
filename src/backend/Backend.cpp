#include <Backend.hpp>

BackendBase* Backend::getBackend(BackendBase::LibType lib, BackendBase::WMType wm){

	BackendBase *newBackend = nullptr;

	// Vulkan
	#ifdef USE_VULKAN
	if(lib == BackendBase::LibType::AUTO_LIB || lib == BackendBase::LibType::VULKAN_LIB){
		try{
			initVK(newBackend, lib, wm);
		}catch(std::exception e){
			if(lib == BackendBase::LibType::VULKAN_LIB){
				throw e;
			}else{
				std::cerr << "Vulkan init error: " << e.what() << " Try OpenGL." << std::endl;
			}
		}
	}
	#endif

	// OpenGL
	#ifdef USE_OpenGL
	if(lib == BackendBase::LibType::AUTO || lib == BackendBase::LibType::OPENGL){
		
	}
	#endif

	return newBackend;
}

BackendBase::~BackendBase(){
	// Vulkan
	#ifdef USE_VULKAN
	vkDestroyInstance(vkInstance, nullptr);
	#endif
}