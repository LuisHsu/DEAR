#ifndef DEAR_SVG_PARSER_DEF
#define DEAR_SVG_PARSER_DEF

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>

#include "element/elements.hpp"

#define DEAR_SVG_ENTER_FUNC_MAP(ELEM) { #ELEM , SVG ## ELEM ## Element::parseEnter }
#define DEAR_SVG_EXIT_FUNC_MAP(ELEM) { #ELEM , SVG ## ELEM ## Element::parseExit }

class SVGParser {
public:
	SVGParser(const char *filename);
	SVGElement *parse();
private:
	xmlDocPtr svgDoc;
	std::unordered_map<std::string, void (*)(xmlDocPtr, xmlNodePtr, SVGElement* &)> enterFuncMap;
	std::unordered_map<std::string, SVGElement* (*)(xmlDocPtr, xmlNodePtr, SVGElement*)> exitFuncMap;
	SVGElement* traverse(xmlNodePtr element, SVGElement* parent);
};
#endif