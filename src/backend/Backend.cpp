#include <Backend.hpp>

BackendBase *Backend::getBackend(BackendBase::LibType lib){
	BackendBase *ret = nullptr;
	// Vulkan
	#ifdef USE_VULKAN
	if(lib == BackendBase::LibType::AUTO_LIB || lib == BackendBase::LibType::VULKAN){
		#ifdef XCB_SUPPORT
		try{
			ret = new BackendXcbVK();
		}catch(std::exception e){
			std::cerr << "Backend Vulkan XCB init error: " << e.what() << ". Try display." << std::endl;
			delete ret;
			ret = nullptr;
		}catch(const char *e){
			std::cerr << "Backend Vulkan XCB init error: " << e << ". Try display." << std::endl;
			delete ret;
			ret = nullptr;
		}
		#endif
		if(!ret){
			try{
				ret = new BackendDisplayVK();
			}catch(std::exception e){
				std::cerr << "Backend Vulkan display init error: " << e.what() << ". Try OpenGL." << std::endl;
				delete ret;
				ret = nullptr;
			}catch(const char *e){
				std::cerr << "Backend Vulkan display init error: " << e << ". Try OpenGL." << std::endl;
				delete ret;
				ret = nullptr;
			}
		}
	}
	#endif
	// OpenGL
	if(!ret){
		#ifdef XCB_SUPPORT
		try{
			ret = new BackendXcbGL();
		}catch(std::exception e){
			std::cerr << "Backend OpenGL XCB init error: " << e.what() << ". Try display." << std::endl;
			delete ret;
			ret = nullptr;
		}catch(const char *e){
			std::cerr << "Backend OpenGL XCB init error: " << e << ". Try display." << std::endl;
			delete ret;
			ret = nullptr;
		}
		#endif
		if(!ret){
			try{
				ret = new BackendDisplayGL();
				lib = BackendBase::LibType::OPENGL;
			}catch(std::exception e){
				delete ret;
				ret = nullptr;
				throw "No supported graphics library.";
			}catch(const char *e){
				delete ret;
				ret = nullptr;
				throw "No supported graphics library.";
			}
		}
	}
	return ret;
}
