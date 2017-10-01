include_directories(. .. ../area)

add_custom_target(greeter_shader ALL
	COMMAND ${GLSLANG_VALIDATOR} -V Greeter.frag -o ${PROJECT_BINARY_DIR}/frag/Greeter.frag.spv
	COMMAND ${GLSLANG_VALIDATOR} -V Greeter.vert -o ${PROJECT_BINARY_DIR}/vert/Greeter.vert.spv
	WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
)

add_executable(dear-greeter
	main.cpp
	Greeter.cpp
	../area/Server.cpp
)