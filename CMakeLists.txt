cmake_minimum_required(VERSION 2.6)
project(dear)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Werror")
set(CMAKE_DEBUG_POSTFIX "_d")

option(USE_VULKAN "Use Vulkan as graphics library" ON)
if(USE_VULKAN)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lvulkan")
add_definitions(-DUSE_VULKAN)
endif(USE_VULKAN)

option(USE_OPENGL "Use OpenGL as graphics library" ON)
if(USE_OPENGL)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lGL")
add_definitions(-DUSE_OPENGL)
endif(USE_OPENGL)

add_subdirectory(src/backend)
include_directories(src src/backend)
add_executable(dear src/main.cpp)

target_link_libraries(dear dear_Backend)