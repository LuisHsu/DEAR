#include <Backend.hpp>

BackendBase *Backend::getBackend(){
	BackendBase *ret = nullptr;
	#ifdef XCB_SUPPORT
	try{
		ret = new BackendXcbVK();
	}catch(std::exception e){
		std::cerr << "Backend Vulkan XCB init error: " << e.what() << ". Try display." << std::endl;
		ret = nullptr;
	}catch(const char *e){
		std::cerr << "Backend Vulkan XCB init error: " << e << ". Try display." << std::endl;
		ret = nullptr;
	}
	#endif
	if(!ret){
		try{
			ret = new BackendDisplayVK();
		}catch(std::exception e){
			std::cerr << "Backend Vulkan display init error: " << e.what() << std::endl;
			ret = nullptr;
			throw e.what();
		}catch(const char *e){
			std::cerr << "Backend Vulkan display init error: " << e << std::endl;
			ret = nullptr;
			throw e;
		}
	}
	return ret;
}
