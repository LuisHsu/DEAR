#include <Backend.hpp>

Backend::Backend(LibType lib):
	lib(lib)
{
	// Vulkan
	#ifdef USE_VULKAN
	if(lib == AUTO || lib == VULKAN){
		try{
			initVK();
		}catch(std::exception e){
			std::cerr << "Backend init error: " << e.what() << " Try OpenGL." << std::endl;
		}
	}
	#endif
	// OpenGL
}

Backend::~Backend(){
	#ifdef USE_VULKAN
	vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
	vkDestroyDevice(vkDevice, nullptr);
	vkDestroyInstance(vkInstance, nullptr);
	#endif
}