#include <Backend.hpp>

BackendBase *Backend::getBackend(BackendBase::LibType lib){
	BackendBase *ret = nullptr;
	// Vulkan
	#ifdef USE_VULKAN
	if(lib == BackendBase::LibType::AUTO || lib == BackendBase::LibType::VULKAN){
		try{
			ret = new BackendVK();
			lib = BackendBase::LibType::VULKAN;
		}catch(std::exception e){
			std::cerr << "Backend init error: " << e.what() << ". Try OpenGL." << std::endl;
		}catch(const char *e){
			std::cerr << "Backend init error: " << e << ". Try OpenGL." << std::endl;
		}
	}
	#endif
	// OpenGL
	if(lib == BackendBase::LibType::AUTO || lib == BackendBase::LibType::OPENGL){
		ret = new BackendGL();
		lib = BackendBase::LibType::OPENGL;
	}
	return ret;
}
