#include "attributes.hpp"
#include "element/elements.hpp"

void SVGAttribute::parseAccentHeight(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr){
	attr_accent_height *elem = (attr_accent_height *) elemPtr;
	std::stringstream ss(value);
	ss >> elem->accentHeightAttr; 
}
void SVGAttribute::parseAlphabetic(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr){
	attr_alphabetic *elem = (attr_alphabetic *) elemPtr;
	std::stringstream ss(value);
	ss >> elem->alphabeticAttr; 
}
void SVGAttribute::parseId(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr){
	attr_id *elem = (attr_id *) elemPtr;
	elem->idAttr = value;
}
void SVGAttribute::parseClass(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr){
	attr_class *elem = (attr_class *) elemPtr;
	elem->classAttr = value;
}
void SVGAttribute::parsePreserveXmlSpace(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr){
	attr_preserveXmlSpace *elem = (attr_preserveXmlSpace *) elemPtr;
	elem->preserveXmlSpaceAttr = (value == "preserve");
}