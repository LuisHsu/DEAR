#ifndef DEAR_SVG_RENDERER_DEF
#define DEAR_SVG_RENDERER_DEF

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <stack>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <SkCanvas.h>
#include <SkPictureRecorder.h>
#include <SkPicture.h>

class SVGRenderer{
public: 
	SVGRenderer(const char *filename, double pixelPerMM);
	void render(SkCanvas *canvas);

private:
	class RecorderClass{
	public:
		SkCanvas *canvas;
		SkPictureRecorder *recorder;
	};
	double pixelPerMM;
	std::stack<RecorderClass> recorderStack;
	xmlDocPtr svgDoc;
	std::unordered_map<std::string, void (*)(SVGRenderer *, xmlNodePtr)> elementFuncMap;
	
	void traverse(xmlNodePtr element);
	static void elem_svg_enter(SVGRenderer *renderer, xmlNodePtr element);
	static void elem_svg_exit(SVGRenderer *renderer, xmlNodePtr element);
};
#endif