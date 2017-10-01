include_directories(. ..)

if(XCB_SUPPORT)
set(BACKENDVK "BackendXcbVK.cpp")
endif(XCB_SUPPORT)
set(BACKENDVK
	${BACKENDVK}
	BackendDisplayVK.cpp
)

add_custom_target(backend_shader ALL
	COMMAND ${GLSLANG_VALIDATOR} -V BackendVK.frag -o ${PROJECT_BINARY_DIR}/frag/BackendVK.frag.spv
	COMMAND ${GLSLANG_VALIDATOR} -V BackendVK.vert -o ${PROJECT_BINARY_DIR}/vert/BackendVK.vert.spv
	WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
)
add_executable(dearDM
	main.cpp
	AreaNode.cpp
	Config.cpp
	Backend.cpp
	BackendBase.cpp
	../IPCMessage.cpp
	${BACKENDVK}
)
