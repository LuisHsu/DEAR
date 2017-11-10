#ifndef DEAR_SVG_RENDERER_DEF
#define DEAR_SVG_RENDERER_DEF

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <stack>

#include <rapidxml_ns.hpp>
#include <SkCanvas.h>
#include <SkPictureRecorder.h>
#include <SkPicture.h>

class SVGRenderer{
public: 
	SVGRenderer(const char *filename);
	void render(SkCanvas *canvas);

private:
	class RecorderClass{
	public:
		SkCanvas *canvas;
		SkPictureRecorder *recorder;
	};
	std::stack<RecorderClass> recorderStack;
	rapidxml_ns::xml_document<> svgDoc;
	std::unordered_map<std::string, void (*)(SVGRenderer *renderer, rapidxml_ns::xml_node<> *)> elementFuncMap;
	
	void traverse(rapidxml_ns::xml_node<> *svg_element);
	static void elem_svg_enter(SVGRenderer *renderer, rapidxml_ns::xml_node<> *element);
	static void elem_svg_exit(SVGRenderer *renderer, rapidxml_ns::xml_node<> *element);
};
#endif