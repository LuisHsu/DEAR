include_directories(. ../src)

add_library(maindock MODULE
	maindock.cpp
)
target_link_libraries(maindock
	${SKIA_LIBRARIES}
)
set_target_properties(maindock PROPERTIES
	OUTPUT_NAME "maindock"
	PREFIX ""
	DEBUG_POSTFIX ""
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/mod/user"
)
add_custom_target(maindock_shader ALL
	COMMAND ${GLSLANG_VALIDATOR} -V maindock.frag -o ${CMAKE_BINARY_DIR}/frag/maindock.frag.spv
	COMMAND ${GLSLANG_VALIDATOR} -V maindock.vert -o ${CMAKE_BINARY_DIR}/vert/maindock.vert.spv
	WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
)