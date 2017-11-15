#ifndef DEAR_SVG_RENDERER_DEF
#define DEAR_SVG_RENDERER_DEF

#include <stack>

#include <SkCanvas.h>
#include <SkPictureRecorder.h>
#include <SkPicture.h>

#include "parser.hpp"

class SVGRenderer{
public: 
	SVGRenderer(const char *filename, double pixelPerMM);
	void render(SkCanvas *canvas);
private:
	double pixelPerMM;
	class RecorderClass{
	public:
		SkCanvas *canvas;
		SkPictureRecorder *recorder;
	};
	SVGElement *tree;
	std::stack<RecorderClass> recorderStack;

};
#endif