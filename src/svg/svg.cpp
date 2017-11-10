#include "renderer.hpp"

void SVGRenderer::elem_svg_enter(SVGRenderer *renderer, rapidxml_ns::xml_node<> *element){
	SkPictureRecorder *recorder = new SkPictureRecorder;
	SkCanvas* newCanvas = recorder->beginRecording(SkIntToScalar(300), SkIntToScalar(300));
	renderer->recorderStack.push({newCanvas, recorder});
	newCanvas->drawColor(SK_ColorBLUE);
}

void SVGRenderer::elem_svg_exit(SVGRenderer *renderer, rapidxml_ns::xml_node<> *element){
	RecorderClass recorderObj = renderer->recorderStack.top();
	renderer->recorderStack.pop();
	sk_sp<SkPicture> picture = recorderObj.recorder->finishRecordingAsPicture();
	SkCanvas* lastCanvas = renderer->recorderStack.top().canvas;
	SkRect rect = picture.get()->cullRect();
	lastCanvas->clipRect(picture.get()->cullRect(), SkClipOp::kIntersect, true);
	lastCanvas->drawPicture(picture.get());
	delete recorderObj.recorder;
}