#include "svgElem.hpp"

SVGsvgElement::SVGsvgElement():SVGElement(attrParseMap_t({
	{"id", parseIdAttr}
})){
}
void SVGsvgElement::parseEnter(xmlDocPtr doc, xmlNodePtr element, SVGElement* &elemPtr){
	elemPtr = new SVGsvgElement;
	elemPtr->parseAttribute(doc, element);
}
SVGElement* SVGsvgElement::parseExit(xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr){
	return elemPtr;
}

void SVGsvgElement::parseIdAttr(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemObj){
	id = value;
}