#include "parser.hpp"

SVGParser::SVGParser(const char *filename):
	enterFuncMap({ 
		DEAR_SVG_ENTER_FUNC_MAP(svg) 
	}),
	exitFuncMap({
		DEAR_SVG_EXIT_FUNC_MAP(svg)
	})
{
	svgDoc = xmlParseFile(filename);
}


SVGElement* SVGParser::parse(){
	try{
		return traverse(xmlDocGetRootElement(svgDoc), nullptr);
	}catch (std::exception const & e){
		std::cerr << "Error loading SVG: " << e.what() << std::endl;
		return nullptr;
	}
}

SVGElement* SVGParser::traverse(xmlNodePtr element, SVGElement* parent){
	std::string nameStr((char *)element->name);
	// Enter
	SVGElement *elemObj = nullptr;
	if(enterFuncMap.find(nameStr) != enterFuncMap.end()){
		enterFuncMap[nameStr](svgDoc, element, elemObj);
	}
	if(elemObj){
		elemObj->parent = parent;
		elemObj->name = nameStr;
	}
	// Child
	xmlNodePtr curElem = xmlFirstElementChild(element);
	while(curElem){
		if(curElem->type == XML_ELEMENT_NODE){
			if(elemObj){
				SVGElement *childObj = traverse(curElem, elemObj);
				if(childObj){
					elemObj->children.push_back(childObj);
				}
			}
		}
		curElem = curElem->next;
	}
	// Exit
	if(exitFuncMap.find(nameStr) != exitFuncMap.end()){
		return exitFuncMap[nameStr](svgDoc, element, elemObj);
	}else{
		return elemObj;
	}
}