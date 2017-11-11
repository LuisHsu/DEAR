#include "renderer.hpp"

SVGRenderer::SVGRenderer(const char *filename, double pixelPerMM):
	pixelPerMM(pixelPerMM), elementFuncMap({{"svg_enter", elem_svg_enter}, {"svg_exit", elem_svg_exit}})
{
	svgDoc = xmlParseFile(filename);
}

void SVGRenderer::render(SkCanvas *canvas){
	recorderStack.push({canvas, nullptr});
	try{
		traverse(xmlDocGetRootElement(svgDoc));
	}catch (std::exception const & e){
		std::cerr << "Error loading SVG: " << e.what() << std::endl;
	}
}

void SVGRenderer::traverse(xmlNodePtr element){
	std::string nameStr((char *)element->name);
	// Enter
	if(elementFuncMap.find(nameStr + "_enter") != elementFuncMap.end()){
		elementFuncMap[nameStr + "_enter"](this, element);
	}
	// Child
	xmlNodePtr curElem = xmlFirstElementChild(element);
	while(curElem){
		if(curElem->type == XML_ELEMENT_NODE){
			traverse(curElem);
		}
		curElem = curElem->next;
	}
	// Exit
	if(elementFuncMap.find(nameStr + "_exit") != elementFuncMap.end()){
		elementFuncMap[nameStr + "_exit"](this, element);
	}
}