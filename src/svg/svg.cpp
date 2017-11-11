#include "renderer.hpp"

void SVGRenderer::elem_svg_enter(SVGRenderer *renderer, xmlNodePtr element){
	SkPictureRecorder *recorder = new SkPictureRecorder;
	SkCanvas* newCanvas = recorder->beginRecording(SkIntToScalar(300),SkIntToScalar(300));
	renderer->recorderStack.push({newCanvas, recorder});
}

void SVGRenderer::elem_svg_exit(SVGRenderer *renderer, xmlNodePtr element){
	// Draw to last canvas and free
	RecorderClass recorderObj = renderer->recorderStack.top();
	recorderObj.canvas->flush();
	renderer->recorderStack.pop();
	sk_sp<SkPicture> picture = recorderObj.recorder->finishRecordingAsPicture();
	SkCanvas* lastCanvas = renderer->recorderStack.top().canvas;
	lastCanvas->clipRect(picture.get()->cullRect(), SkClipOp::kIntersect, true);
	lastCanvas->drawPicture(picture.get());
	lastCanvas->flush();
	delete recorderObj.recorder;
}