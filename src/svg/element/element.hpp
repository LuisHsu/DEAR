#ifndef DEAR_SVG_ELEMENT
#define DEAR_SVG_ELEMENT

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "../attributes.hpp"

#define attrParseMap_t std::unordered_map<std::string,void(*)(std::string,xmlDocPtr,xmlNodePtr,SVGElement*)>

class SVGElement:
	public SVGAttribute::attr_id,
	public SVGAttribute::attr_preserveXmlSpace
{
public:
	SVGElement(attrParseMap_t attrParseMap);
	SVGElement *parent = nullptr;
	std::vector<SVGElement *> children;
	attrParseMap_t attrParseMap;
	
	// Global attributes
	std::string name;

	void parseAttribute(xmlDocPtr doc, xmlNodePtr element);

	/*  Element instance class should define these function :
	 *	static void parseEnter(xmlDocPtr doc, xmlNodePtr element, SVGElement* &elemPtr); // MUST alloc instance
	 *	static SVGElement* parseExit(xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr);
	 */
};
#endif