include_directories(. ..)
add_library(skybox MODULE
	skybox.cpp
)
set_target_properties(skybox PROPERTIES
	OUTPUT_NAME "skybox"
	PREFIX ""
	DEBUG_POSTFIX ""
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/mod/area"
)