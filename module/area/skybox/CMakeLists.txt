include_directories(. ../src)

find_package(PNG REQUIRED)
if(PNG_FOUND)
include_directories(${PNG_INCLUDE_DIR})
endif(PNG_FOUND)

add_library(skybox MODULE
	skybox.cpp
)
target_link_libraries(skybox
	${PNG_LIBRARIES}
)
set_target_properties(skybox PROPERTIES
	OUTPUT_NAME "skybox"
	PREFIX ""
	DEBUG_POSTFIX ""
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/mod/area"
)
add_custom_target(skybox_shader ALL
	COMMAND ${GLSLANG_VALIDATOR} -V skybox.frag -o ${CMAKE_BINARY_DIR}/frag/skybox.frag.spv
	COMMAND ${GLSLANG_VALIDATOR} -V skybox.vert -o ${CMAKE_BINARY_DIR}/vert/skybox.vert.spv
	WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
)