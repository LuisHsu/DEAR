include_directories(. ../src)

add_library(startmenu MODULE
	startmenu.cpp
)
target_link_libraries(startmenu
	${SKIA_LIBRARIES}
)
set_target_properties(startmenu PROPERTIES
	OUTPUT_NAME "startmenu"
	PREFIX ""
	DEBUG_POSTFIX ""
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/mod/user"
)
add_custom_target(startmenu_shader ALL
	COMMAND ${GLSLANG_VALIDATOR} -V startmenu.frag -o ${CMAKE_BINARY_DIR}/frag/startmenu.frag.spv
	COMMAND ${GLSLANG_VALIDATOR} -V startmenu.vert -o ${CMAKE_BINARY_DIR}/vert/startmenu.vert.spv
	WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
)