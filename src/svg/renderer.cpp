#include "renderer.hpp"

SVGRenderer::SVGRenderer(const char *filename, double pixelPerMM):
	pixelPerMM(pixelPerMM)
{
	SVGParser parser(filename);
	tree = parser.parse();
}

void SVGRenderer::render(SkCanvas *canvas){

}