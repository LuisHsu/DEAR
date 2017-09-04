include_directories(.)

if(XCB_SUPPORT)
set(XCB_CPP "Xcb.cpp")
else(XCB_SUPPORT)
set(XCB_CPP "")
endif(XCB_SUPPORT)

if(WAYLAND_SUPPORT)
set(WAYLAND_CPP "Wayland.cpp")
else(WAYLAND_SUPPORT)
set(WAYLAND_CPP "")
endif(WAYLAND_SUPPORT)

add_library(dear_Backend
	Backend.cpp
	VKinit.cpp
	Display.cpp
	${XCB_CPP}
	${WAYLAND_CPP}
)