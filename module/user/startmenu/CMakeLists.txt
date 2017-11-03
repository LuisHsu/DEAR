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