#ifndef BACKEND_BASE_DEF
#define BACKEND_BASE_DEF

#include <vector>

#ifdef USE_VULKAN
#include <vulkan/vulkan.h>
#endif

class BackendBase {
public:
	enum LibType {AUTO_LIB, OPENGL_LIB, VULKAN_LIB};
	enum WMType {AUTO_WM, XCB_WM, WAYLAND_WM, DISPLAY_WM};

	LibType libType;
	WMType wmType;

#ifdef USE_VULKAN
	VkInstance vkInstance;
	VkPhysicalDevice vkPhyDevice;
	int graphicsFamily = -1;
	int presentFamily = -1;
#endif
	BackendBase(LibType lib):libType(lib){};
	~BackendBase();
};

#endif