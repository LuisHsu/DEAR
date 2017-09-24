include_directories(. .. ../area)

add_executable(dear-greeter
	main.cpp
	Greeter.cpp
	../area/Server.cpp
)