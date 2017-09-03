#ifndef BACKEND_BASE_DEF
#define BACKEND_BASE_DEF

#include <vector>

#ifdef USE_VULKAN
#include <vulkan/vulkan.h>
struct PhysicalDevice{
	VkPhysicalDevice device;
	int graphicsFamily;
	int presentFamily;
};
#endif

class BackendBase {
public:
	enum LibType {AUTO_LIB, OPENGL_LIB, VULKAN_LIB};
	enum WMType {AUTO_WM, XCB_WM, WAYLAND_WM, DISPLAY_WM};

#ifdef USE_VULKAN
	VkInstance vkInstance;
	std::vector<PhysicalDevice> vkDevices;
#endif	

private:
	// run()
};

#endif