include_directories(. ..)
add_executable(dear-session
	localMain.cpp
	session.cpp
)
target_link_libraries(dear-session
	dearMsg
)