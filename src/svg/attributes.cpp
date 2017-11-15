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
void SVGAttribute::parseViewTarget(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr){
	attr_viewTarget *elem = (attr_viewTarget *) elemPtr;
	elem->viewTargetAttr = value;
}
void SVGAttribute::parseXChannelSelector(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr){
	attr_xChannelSelector *elem = (attr_xChannelSelector *) elemPtr;
	if(value == "R"){
		elem->xChannelSelectorAttr = attr_xChannelSelector::XChannel::R;
	}else if(value == "G"){
		elem->xChannelSelectorAttr = attr_xChannelSelector::XChannel::G;
	}else if(value == "B"){
		elem->xChannelSelectorAttr = attr_xChannelSelector::XChannel::B;
	}else if(value == "A"){
		elem->xChannelSelectorAttr = attr_xChannelSelector::XChannel::A;
	}
}
void SVGAttribute::parsePreserveXmlSpace(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr){
	attr_preserveXmlSpace *elem = (attr_preserveXmlSpace *) elemPtr;
	elem->preserveXmlSpaceAttr = (value == "preserve");
}
void SVGAttribute::parseYChannelSelector(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr){
	attr_yChannelSelector *elem = (attr_yChannelSelector *) elemPtr;
	if(value == "R"){
		elem->yChannelSelectorAttr = attr_yChannelSelector::YChannel::R;
	}else if(value == "G"){
		elem->yChannelSelectorAttr = attr_yChannelSelector::YChannel::G;
	}else if(value == "B"){
		elem->yChannelSelectorAttr = attr_yChannelSelector::YChannel::B;
	}else if(value == "A"){
		elem->yChannelSelectorAttr = attr_yChannelSelector::YChannel::A;
	}
}
void SVGAttribute::parseZ(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr){
	attr_z *elem = (attr_z *) elemPtr;
	std::stringstream ss(value);
	ss >> elem->zAttr; 
}
void SVGAttribute::parseZoomAndPan(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr){
	attr_zoomAndPan *elem = (attr_zoomAndPan *) elemPtr;
	elem->zoomAndPanAttr = (value == "magnify");
}
void SVGAttribute::parseY2(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr){
	attr_y2 *elem = (attr_y2 *)elemPtr;
	elem->y2Attr = value;
}
void SVGAttribute::parseY1(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr){
	attr_y1 *elem = (attr_y1 *)elemPtr;
	elem->y1Attr = value;
}
void SVGAttribute::parseY(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr){
	attr_y *elem = (attr_y *)elemPtr;
	elem->yAttr = value;
}
void SVGAttribute::parseX2(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr){
	attr_x2 *elem = (attr_x2 *)elemPtr;
	elem->x2Attr = value;
}
void SVGAttribute::parseX1(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr){
	attr_x1 *elem = (attr_x1 *)elemPtr;
	elem->x1Attr = value;
}
void SVGAttribute::parseX(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr){
	attr_x *elem = (attr_x *)elemPtr;
	elem->xAttr = value;
}
void SVGAttribute::parseXHeight(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr){
	attr_x_height *elem = (attr_x_height *) elemPtr;
	std::stringstream ss(value);
	ss >> elem->xHeightAttr; 
}
void SVGAttribute::parseWidth(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr){
	attr_width *elem = (attr_width *) elemPtr;
	elem->widthAttr = value;
}
void SVGAttribute::parseViewBox(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr){
	attr_viewBox *elem = (attr_viewBox *) elemPtr;
	elem->viewBoxAttr = value;
}
void SVGAttribute::parseVertAdvY(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr){

}
void SVGAttribute::parseVertOriginX(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr){

}
void SVGAttribute::parseVertOriginY(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr){

}