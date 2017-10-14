include_directories(. ..)
add_executable(dear-area
	areaMain.cpp
	area.cpp
)
target_link_libraries(dear-area
	dearMsg
)