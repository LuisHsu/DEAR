#ifndef DEAR_SVG_ELEMENT
#define DEAR_SVG_ELEMENT

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include <libxml/parser.h>
#include <libxml/tree.h>

#define attrParseMap_t std::unordered_map<std::string,void(*)(std::string,xmlDocPtr,xmlNodePtr,SVGElement*)>

class SVGElement{
public:
	SVGElement(attrParseMap_t attrParseMap);
	SVGElement *parent = nullptr;
	std::vector<SVGElement *> children;
	attrParseMap_t attrParseMap;

	std::string id;
	std::string name;
	bool preserveXmlSpace;
	
	void parseAttribute(xmlDocPtr doc, xmlNodePtr element);
	/*  Element instance class should define these function
	 *
	 *	static void parseEnter(xmlDocPtr doc, xmlNodePtr element, SVGElement* &elemPtr); // MUST alloc instance
	 *	static SVGElement* parseExit(xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr);
	 * /
};
#endif