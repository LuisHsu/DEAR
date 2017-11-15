#ifndef DEAR_SVG_SVG_ELEMENT
#define DEAR_SVG_SVG_ELEMENT

#include <string>

#include "element.hpp"

class SVGsvgElement : public SVGElement,
	public SVGAttribute::attr_class
{
public:
	SVGsvgElement();
	// Attribute

	// Parser function
	static void parseEnter(xmlDocPtr doc, xmlNodePtr element, SVGElement* &elemPtr);
	static SVGElement* parseExit(xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr);
};
#endif