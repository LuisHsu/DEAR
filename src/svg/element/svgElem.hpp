#ifndef DEAR_SVG_SVG_ELEMENT
#define DEAR_SVG_SVG_ELEMENT

#include "element.hpp"

class SVGsvgElement : public SVGElement{
public:
	SVGsvgElement();
	// Attribute
	static void parseIdAttr(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemObj);
	// Parser function
	static void parseEnter(xmlDocPtr doc, xmlNodePtr element, SVGElement* &elemPtr);
	static SVGElement* parseExit(xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr);
};
#endif