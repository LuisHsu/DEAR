add_executable(dear-session
	localMain.cpp
	session.cpp
	input.cpp
	codemap.cpp
)
target_link_libraries(dear-session
	dearMsg
	${LIBINPUT_LIBRARIES}
	${LIBUDEV_LIBRARIES}
	${LIBSYSTEMD_LIBRARIES}
)