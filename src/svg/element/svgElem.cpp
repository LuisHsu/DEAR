#include "svgElem.hpp"

SVGsvgElement::SVGsvgElement():SVGElement(attrParseMap_t({
	{"id", SVGAttribute::parseId},
	{"space", SVGAttribute::parsePreserveXmlSpace},
	{"class", SVGAttribute::parseClass},
})){
}
void SVGsvgElement::parseEnter(xmlDocPtr doc, xmlNodePtr element, SVGElement* &elemPtr){
	elemPtr = new SVGsvgElement;
	elemPtr->parseAttribute(doc, element);
}
SVGElement* SVGsvgElement::parseExit(xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr){
	return elemPtr;
}