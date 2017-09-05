#include <Xcb.hpp>

BackendXcb::BackendXcb(BackendBase::LibType lib):
	BackendBase(lib)
{
	// Connection
	int screenp = 0;
	connection = xcb_connect(nullptr, &screenp);
	switch(xcb_connection_has_error(connection)){
		case XCB_CONN_ERROR:
			throw "[XCB Backend] Socket errors, pipe errors or other stream errors.";
		break;
		case XCB_CONN_CLOSED_EXT_NOTSUPPORTED:
			throw "[XCB Backend] Extension not supported.";
		break;
		case XCB_CONN_CLOSED_MEM_INSUFFICIENT:
			throw "[XCB Backend] Memory not available.";
		break;
		case XCB_CONN_CLOSED_REQ_LEN_EXCEED:
			throw "[XCB Backend] Exceeding request length that server accepts.";
		break;
		case XCB_CONN_CLOSED_PARSE_ERR:
			throw "[XCB Backend] Error during parsing display string.";
		break;
		case XCB_CONN_CLOSED_INVALID_SCREEN:
			throw "[XCB Backend] Server does not have a screen matching the display.";
		break;
		default:
		break;
	}
	// Screen
	xcb_screen_iterator_t iter = xcb_setup_roots_iterator(xcb_get_setup(connection));
	for (int s = screenp; s > 0; --s){
		xcb_screen_next(&iter);
	}
	screen = iter.data;
	// Window
	window = xcb_generate_id(connection);
	uint32_t eventMask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
	uint32_t valList[] = {
		screen->black_pixel,
		XCB_EVENT_MASK_NO_EVENT
	};
	xcb_create_window(
		connection,
		XCB_COPY_FROM_PARENT,
		window,
		screen->root,
		Config::dispX,
		Config::dispY,
		(Config::dispWidth == 0) ? screen->width_in_pixels : Config::dispWidth,
		(Config::dispHeight == 0) ? screen->height_in_pixels : Config::dispHeight,
		Config::dispBorder,
		XCB_WINDOW_CLASS_INPUT_OUTPUT,
		screen->root_visual,
		eventMask,
		valList
	);
	// Window title
	xcb_change_property(
		connection,
		XCB_PROP_MODE_REPLACE,
		window,
		XCB_ATOM_WM_NAME,
		XCB_ATOM_STRING,
		8,
		strlen(Config::dispTitle),
		Config::dispTitle);
	xcb_map_window(connection, window);
	xcb_flush(connection);
}

BackendXcb::~BackendXcb(){
	#ifdef USE_VULKAN
	vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
	vkDestroyInstance(vkInstance, nullptr);
	#endif
	xcb_disconnect(connection);
}

#ifdef USE_VULKAN
void BackendXcb::createSurface(){
	VkXcbSurfaceCreateInfoKHR surfaceCreateInfo = {};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.pNext = NULL;
	surfaceCreateInfo.flags = 0;
	surfaceCreateInfo.connection = connection;
	surfaceCreateInfo.window = window;
	switch(vkCreateXcbSurfaceKHR(vkInstance, &surfaceCreateInfo, NULL, &vkSurface)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Create display surface] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Create display surface] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		default:
		break;
	}
}
#endif