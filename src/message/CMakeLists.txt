add_library(dearMsg
	IPCServer.cpp
	IPCClient.cpp
	TCPServer.cpp
)
target_link_libraries(dearMsg
	${LIBUV_LIBRARIES}
)