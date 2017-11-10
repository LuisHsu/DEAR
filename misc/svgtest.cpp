#include <iostream>

#include <SkSurface.h>
#include <SkCanvas.h>
#include <GrContext.h>
#include <SkImageInfo.h>

#include <GLFW/glfw3.h>

#include "../src/svg/renderer.hpp"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

void renderPic(const char *path){
	sk_sp<GrContext> context = GrContext::MakeGL(nullptr);
    SkImageInfo info = SkImageInfo:: MakeN32Premul(WINDOW_WIDTH, WINDOW_HEIGHT);
    sk_sp<SkSurface> gpuSurface(SkSurface::MakeRenderTarget(context.get(), SkBudgeted::kNo, info));
    if (!gpuSurface) {
        SkDebugf("SkSurface::MakeRenderTarget returned null\n");
        return;
    }
    SkCanvas* canvas = gpuSurface->getCanvas();
	SVGRenderer renderer(path);
	renderer.render(canvas);
	canvas->flush();
}

int main(int argc, char *argv[]){
	// GLFW
	GLFWwindow* window;
	if(!glfwInit()){
		return -1;
	}
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Dear-SVGTest", NULL, NULL);
	if(!window){
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);
		renderPic(argv[1]);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}