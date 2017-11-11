cmake_minimum_required(VERSION 3.0)
project(dear)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Werror")
set(CMAKE_DEBUG_POSTFIX "_d")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(GLSLANG_VALIDATOR ${CMAKE_CURRENT_LIST_DIR}/tools/glslangValidator)

# Vulkan
include_directories(${CMAKE_CURRENT_LIST_DIR}/third_party/VulkanTool/include)
set(VULKAN_LIBRARIES ${CMAKE_CURRENT_LIST_DIR}/third_party/VulkanTool/lib/libvulkan.so.1.0.61)

# Skia
include_directories(
	${CMAKE_CURRENT_LIST_DIR}/third_party/Skia/include/android
	${CMAKE_CURRENT_LIST_DIR}/third_party/Skia/include/c
	${CMAKE_CURRENT_LIST_DIR}/third_party/Skia/include/codec
	${CMAKE_CURRENT_LIST_DIR}/third_party/Skia/include/config
	${CMAKE_CURRENT_LIST_DIR}/third_party/Skia/include/core
	${CMAKE_CURRENT_LIST_DIR}/third_party/Skia/include/effects
	${CMAKE_CURRENT_LIST_DIR}/third_party/Skia/include/encode
	${CMAKE_CURRENT_LIST_DIR}/third_party/Skia/include/gpu
	${CMAKE_CURRENT_LIST_DIR}/third_party/Skia/include/pathops
	${CMAKE_CURRENT_LIST_DIR}/third_party/Skia/include/ports
	${CMAKE_CURRENT_LIST_DIR}/third_party/Skia/include/svg
	${CMAKE_CURRENT_LIST_DIR}/third_party/Skia/include/utils
	${CMAKE_CURRENT_LIST_DIR}/third_party/Skia/include/views
)
set(SKIA_LIBRARIES ${CMAKE_CURRENT_LIST_DIR}/third_party/Skia/out/Static/libskia.a)
add_definitions(-DSK_VULKAN)

find_package(Xml REQUIRED)
if(XML_FOUND)
include_directories(${XML_INCLUDE_DIR})
endif(XML_FOUND)

find_package(udev REQUIRED)
if(LIBUDEV_FOUND)
include_directories(${LIBUDEV_INCLUDE_DIR})
endif(LIBUDEV_FOUND)

find_package(Systemd REQUIRED)
if(LIBSYSTEMD_FOUND)
include_directories(${LIBSYSTEMD_INCLUDE_DIR})
endif(LIBSYSTEMD_FOUND)

find_package(Libuv REQUIRED)
if(LIBUV_FOUND)
include_directories(${LIBUV_INCLUDE_DIR})
endif(LIBUV_FOUND)

find_package(Input REQUIRED)
if(LIBINPUT_FOUND)
include_directories(${LIBINPUT_INCLUDE_DIR})
endif(LIBINPUT_FOUND)

option(XCB_SUPPORT "Support XCB" ON)
if(XCB_SUPPORT)
find_package(Xcb REQUIRED)
if(XCB_FOUND)
include_directories(${XCB_INCLUDE_DIR})
add_definitions(-DXCB_SUPPORT)
add_definitions(-DVK_USE_PLATFORM_XCB_KHR)
endif(XCB_FOUND)
endif(XCB_SUPPORT)

file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/frag)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/vert)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/mod)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/res)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/mod/area)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/mod/user)

include_directories(src)
add_subdirectory(src/message)
add_subdirectory(src/svg)
add_subdirectory(src/area)
add_subdirectory(src/session)

option(DEAR_AREA_MODULES "Build area module" ON)
if(DEAR_AREA_MODULES)
add_subdirectory(module/area)
endif(DEAR_AREA_MODULES)

option(DEAR_USER_MODULES "Build user module" ON)
if(DEAR_USER_MODULES)
add_subdirectory(module/user)
endif(DEAR_USER_MODULES)