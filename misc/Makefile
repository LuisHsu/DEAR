CXX = g++
CFLAGS = -std=c++11 -g -DSK_VULKAN

svgtest: svgtest.cpp
	$(CXX) $(CFLAGS) -o $@ \
	-I/usr/include/libxml2 \
	-I../third_party/VulkanTool/include \
	-I../third_party/Skia/include \
	-I../third_party/Skia/include/android \
	-I../third_party/Skia/include/c \
	-I../third_party/Skia/include/codec \
	-I../third_party/Skia/include/config \
	-I../third_party/Skia/include/core \
	-I../third_party/Skia/include/effects \
	-I../third_party/Skia/include/encode \
	-I../third_party/Skia/include/gpu \
	-I../third_party/Skia/include/pathops \
	-I../third_party/Skia/include/ports \
	-I../third_party/Skia/include/svg \
	-I../third_party/Skia/include/utils \
	-I../third_party/Skia/include/views \
	-L../third_party/Skia/out/Static \
	-L../build/src/svg \
	svgtest.cpp -ldearSVG_d -lskia -lpthread -lpng -lGL -lglfw -lfontconfig -lfreetype -ldl -lxml2