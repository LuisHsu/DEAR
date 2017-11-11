#include <iostream>
#include <cstdio>

#include <SkSurface.h>
#include <SkCanvas.h>
#include <GrContext.h>
#include <GrBackendSurface.h>

#include <GLFW/glfw3.h>

#include "../src/svg/renderer.hpp"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

void renderPic(const char *path, SkCanvas* canvas){
	// Get pixel per millimeter
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	int phyWidth, phyHeight;
	glfwGetMonitorPhysicalSize(monitor, &phyWidth, &phyHeight);

	SVGRenderer renderer(path,(double) glfwGetVideoMode(monitor)->width / (double) phyWidth);
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

	GrContext *grContext = GrContext::MakeGL(nullptr).release();
	GrGLFramebufferInfo framebufferInfo;
    framebufferInfo.fFBOID = 0;
    GrBackendRenderTarget backendRenderTarget(WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, kSkia8888_GrPixelConfig, framebufferInfo);
	SkSurface *gpuSurface = SkSurface::MakeFromBackendRenderTarget(grContext, backendRenderTarget, kBottomLeft_GrSurfaceOrigin, nullptr, nullptr).release();
	SkCanvas* gpuCanvas = gpuSurface->getCanvas();

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);
		renderPic(argv[1], gpuCanvas);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}