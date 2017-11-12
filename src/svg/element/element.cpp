#include "element.hpp"

SVGElement::SVGElement(attrParseMap_t attrParseMap):
	attrParseMap(attrParseMap){
}

void SVGElement::parseAttribute(xmlDocPtr doc, xmlNodePtr element){
	// Attribute
	xmlAttrPtr attribute = element->properties;
	while(attribute){
		xmlChar* value = xmlNodeListGetString(doc, attribute->children, 1);
		std::string nameStr((char *)attribute->name);
		if(attrParseMap.find(nameStr) != attrParseMap.end()){
			attrParseMap[nameStr]((char *)value, doc, element, this);
		}
		xmlFree(value);
		attribute = attribute->next;
	}
}