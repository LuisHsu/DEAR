cmake_minimum_required(VERSION 3.0)
project(dear)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Werror")
set(CMAKE_DEBUG_POSTFIX "_d")

option(USE_VULKAN "Use Vulkan as graphics library" ON)
if(USE_VULKAN)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lvulkan")
add_definitions(-DUSE_VULKAN)
endif(USE_VULKAN)

option(XCB_SUPPORT "Support XCB" ON)
if(XCB_SUPPORT)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lxcb")
add_definitions(-DXCB_SUPPORT)
add_definitions(-DVK_USE_PLATFORM_XCB_KHR)
endif(XCB_SUPPORT)

add_subdirectory(src/backend)
include_directories(src src/backend)
add_executable(dear 
	src/main.cpp
	src/Config.cpp
)

target_link_libraries(dear dear_Backend)