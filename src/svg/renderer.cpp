#include "renderer.hpp"

SVGRenderer::SVGRenderer(const char *filename):
	elementFuncMap({{"svg_enter", elem_svg_enter}, {"svg_exit", elem_svg_exit}})
{
	// Read file content
	std::ifstream fin(filename, std::ios::ate | std::ios::binary);
	if (!fin.is_open()) {
		throw "[SVG renderer] failed to open file";
	}
	uint32_t codeSize = (size_t)fin.tellg();
	char svgCode[codeSize];
	fin.seekg(0);
	fin.read(svgCode, codeSize);
	fin.close();
	// Parse document
	svgDoc.parse<0>(svgCode);
}

void SVGRenderer::render(SkCanvas *canvas){
	recorderStack.push({canvas, nullptr});
	try{
		traverse(svgDoc.first_node("svg"));
	}catch (std::exception const & e){
		std::cerr << "Error loading SVG: " << e.what() << std::endl;
	}
}

void SVGRenderer::traverse(rapidxml_ns::xml_node<> *svg_element){
	std::string nameStr(svg_element->name());
	// Enter
	if(elementFuncMap.find(nameStr + "_enter") != elementFuncMap.end()){
		elementFuncMap[nameStr + "_enter"](this, svg_element);
	}
	// Child
	// Sibling
	// Exit
	if(elementFuncMap.find(nameStr + "_exit") != elementFuncMap.end()){
		elementFuncMap[nameStr + "_exit"](this, svg_element);
	}
}