include_directories(. ../src ${PROJECT_SOURCE_DIR}/third_party/skia/include)

add_library(startmenu MODULE
	startmenu.cpp
)
target_link_libraries(startmenu
	${PROJECT_SOURCE_DIR}/third_party/skia/libskia.a
)
set_target_properties(startmenu PROPERTIES
	OUTPUT_NAME "startmenu"
	PREFIX ""
	DEBUG_POSTFIX ""
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/mod/user"
)