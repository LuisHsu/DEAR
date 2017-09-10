include_directories(. ..)

if(USE_VULKAN)
set(BACKENDVK "BackendVK.cpp")
set(GLSLANG_VALIDATOR ${CMAKE_CURRENT_LIST_DIR}/../../tools/glslangValidator)
add_custom_target(backend_shader
	COMMAND ${GLSLANG_VALIDATOR} -V BackendVK.frag -o ${PROJECT_BINARY_DIR}/frag/BackendVK.frag.spv
	COMMAND ${GLSLANG_VALIDATOR} -V BackendVK.vert -o ${PROJECT_BINARY_DIR}/vert/BackendVK.vert.spv
	WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
)
endif(USE_VULKAN)

add_library(dear_Backend
	Backend.cpp
	Base.cpp
	BackendGL.cpp
	${BACKENDVK}
)

if(USE_VULKAN)
add_dependencies(dear_Backend
	backend_shader
)
endif(USE_VULKAN)
