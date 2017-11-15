#ifndef DEAR_SVG_ATTRIBUTES_DEF
#define DEAR_SVG_ATTRIBUTES_DEF

#include <string>
#include <sstream>
#include <libxml/parser.h>
#include <libxml/tree.h>

class SVGElement;

namespace SVGAttribute{
/*** Classes ***/
	class attr_accent_height {
		public:
		float accentHeightAttr;
	};
	class attr_alphabetic {
	public:
		float alphabeticAttr;
	};
class attr_amplitude {
public:
	std::string amplitudeAttr;
};
class attr_arabic_form {
public:
	std::string arabicFormAttr;
};
class attr_ascent {
public:
	std::string ascentAttr;
};
class attr_azimuth {
public:
	std::string azimuthAttr;
};
class attr_baseFrequency {
public:
	std::string baseFrequencyAttr;
};
class attr_bbox {
public:
	std::string bboxAttr;
};
class attr_bias {
public:
	std::string biasAttr;
};
class attr_cap_height {
public:
	std::string cap_heightAttr;
};
class attr_class{
public:
	std::string classAttr;
};
class attr_clipPathUnits {
public:
	std::string clipPathUnitsAttr;
};
class attr_cx {
public:
	std::string cxAttr;
};
class attr_cy {
public:
	std::string cyAttr;
};
class attr_d {
public:
	std::string dAttr;
};
class attr_descent {
public:
	std::string descentAttr;
};
class attr_diffuseConstant {
public:
	std::string diffuseConstantAttr;
};
class attr_divisor {
public:
	std::string divisorAttr;
};
class attr_dx {
public:
	std::string dxAttr;
};
class attr_dy {
public:
	std::string dyAttr;
};
class attr_edgeMode {
public:
	std::string edgeModeAttr;
};
class attr_elevation {
public:
	std::string elevationAttr;
};
class attr_exponent {
public:
	std::string exponentAttr;
};
class attr_filterRes {
public:
	std::string filterResAttr;
};
class attr_filterUnits {
public:
	std::string filterUnitsAttr;
};
class attr_font_family {
public:
	std::string fontFamilyAttr;
};
class attr_font_size {
public:
	std::string font_sizeAttr;
};
class attr_font_stretch {
public:
	std::string fontStretchAttr;
};
class attr_font_style {
public:
	std::string fontStyleAttr;
};
class attr_font_variant {
public:
	std::string fontVariantAttr;
};
class attr_font_weight {
public:
	std::string fontWeightAttr;
};
class attr_format {
public:
	std::string formatAttr;
};
class attr_fx {
public:
	std::string fxAttr;
};
class attr_fy {
public:
	std::string fyAttr;
};
class attr_g1 {
public:
	std::string g1Attr;
};
class attr_g2 {
public:
	std::string g2Attr;
};
class attr_glyph_name {
public:
	std::string glyphNameAttr;
};
class attr_glyphRef {
public:
	std::string glyphRefAttr;
};
class attr_gradientTransform {
public:
	std::string gradientTransformAttr;
};
class attr_gradientUnits {
public:
	std::string gradientUnitsAttr;
};
class attr_hanging {
public:
	std::string hangingAttr;
};
class attr_height {
public:
	std::string heightAttr;
};
class attr_horiz_adv_x {
public:
	std::string horizAdv_xAttr;
};
class attr_horiz_origin_x {
public:
	std::string horizOriginXAttr;
};
class attr_horiz_origin_y {
public:
	std::string horizOriginYAttr;
};
class attr_id {
public:
	std::string idAttr;
};
class attr_ideographic {
public:
	std::string ideographicAttr;
};
class attr_in {
public:
	std::string inAttr;
};
class attr_in2 {
public:
	std::string in2Attr;
};
class attr_intercept {
public:
	std::string interceptAttr;
};
class attr_k {
public:
	std::string kAttr;
};
class attr_k1 {
public:
	std::string k1Attr;
};
class attr_k2 {
public:
	std::string k2Attr;
};
class attr_k3 {
public:
	std::string k3Attr;
};
class attr_k4 {
public:
	std::string k4Attr;
};
class attr_kernelMatrix {
public:
	std::string kernelMatrixAttr;
};
class attr_kernelUnitLength {
public:
	std::string kernelUnitLengthAttr;
};
class attr_lang {
public:
	std::string langAttr;
};
class attr_lengthAdjust {
public:
	std::string lengthAdjustAttr;
};
class attr_limitingConeAngle {
public:
	std::string limitingConeAngleAttr;
};
class attr_markerHeight {
public:
	std::string markerHeightAttr;
};
class attr_markerUnits {
public:
	std::string markerUnitsAttr;
};
class attr_markerWidth {
public:
	std::string markerWidthAttr;
};
class attr_maskContentUnits {
public:
	std::string maskContentUnitsAttr;
};
class attr_maskUnits {
public:
	std::string maskUnitsAttr;
};
class attr_mathematical {
public:
	std::string mathematicalAttr;
};
class attr_method {
public:
	std::string methodAttr;
};
class attr_mode {
public:
	std::string modeAttr;
};
class attr_name {
public:
	std::string nameAttr;
};
class attr_numOctaves {
public:
	std::string numOctavesAttr;
};
class attr_offset {
public:
	std::string offsetAttr;
};
class attr_operator {
public:
	std::string operatorAttr;
};
class attr_order {
public:
	std::string orderAttr;
};
class attr_orient {
public:
	std::string orientAttr;
};
class attr_orientation {
public:
	std::string orientationAttr;
};
class attr_overline_position {
public:
	std::string overlinePositionAttr;
};
class attr_overline_thickness {
public:
	std::string overlineThicknessAttr;
};
class attr_panose_1 {
public:
	std::string panoseLAttr;
};
class attr_pathLength {
public:
	std::string pathLengthAttr;
};
class attr_patternContentUnits {
public:
	std::string patternContentUnitsAttr;
};
class attr_patternTransform {
public:
	std::string patternTransformAttr;
};
class attr_patternUnits {
public:
	std::string patternUnitsAttr;
};
class attr_points {
public:
	std::string pointsAttr;
};
class attr_pointsAtX {
public:
	std::string pointsAtXAttr;
};
class attr_pointsAtY {
public:
	std::string pointsAtYAttr;
};
class attr_pointsAtZ {
public:
	std::string pointsAtZAttr;
};
class attr_preserveAlpha {
public:
	std::string preserveAlphaAttr;
};
class attr_preserveAspectRatio {
public:
	std::string preserveAspectRatioAttr;
};
class attr_primitiveUnits {
public:
	std::string primitiveUnitsAttr;
};
class attr_r {
public:
	std::string rAttr;
};
class attr_radius {
public:
	std::string radiusAttr;
};
class attr_refX {
public:
	std::string refXAttr;
};
class attr_refY {
public:
	std::string refYAttr;
};
class attr_restart {
public:
	std::string restartAttr;
};
class attr_result {
public:
	std::string resultAttr;
};
class attr_rotate {
public:
	std::string rotateAttr;
};
class attr_rx {
public:
	std::string rxAttr;
};
class attr_ry {
public:
	std::string ryAttr;
};
class attr_scale {
public:
	std::string scaleAttr;
};
class attr_seed {
public:
	std::string seedAttr;
};
class attr_slope {
public:
	std::string slopeAttr;
};
class attr_spacing {
public:
	std::string spacingAttr;
};
class attr_specularConstant {
public:
	std::string specularConstantAttr;
};
class attr_specularExponent {
public:
	std::string specularExponentAttr;
};
class attr_spreadMethod {
public:
	std::string spreadMethodAttr;
};
class attr_startOffset {
public:
	std::string startOffsetAttr;
};
class attr_stdDeviation {
public:
	std::string stdDeviationAttr;
};
class attr_stemh {
public:
	std::string stemhAttr;
};
class attr_stemv {
public:
	std::string stemvAttr;
};
class attr_stitchTiles {
public:
	std::string stitchTilesAttr;
};
class attr_strikethrough_position {
public:
	std::string strikethroughPositionAttr;
};
class attr_strikethrough_thickness {
public:
	std::string strikethroughThicknessAttr;
};
class attr_string {
public:
	std::string stringAttr;
};
class attr_style {
public:
	std::string styleAttr;
};
class attr_surfaceScale {
public:
	std::string surfaceScaleAttr;
};
class attr_tableValues {
public:
	std::string tableValuesAttr;
};
class attr_targetX {
public:
	std::string targetXAttr;
};
class attr_targetY {
public:
	std::string targetYAttr;
};
class attr_textLength {
public:
	std::string textLengthAttr;
};
class attr_to {
public:
	std::string toAttr;
};
class attr_transform {
public:
	std::string transformAttr;
};
class attr_type {
public:
	std::string typeAttr;
};
class attr_u1 {
public:
	std::string u1Attr;
};
class attr_u2 {
public:
	std::string u2Attr;
};
class attr_underline_position {
public:
	std::string underlinePositionAttr;
};
class attr_underline_thickness {
public:
	std::string underlineThicknessAttr;
};
class attr_unicode {
public:
	std::string unicodeAttr;
};
class attr_unicode_range {
public:
	std::string unicodeRangeAttr;
};
class attr_units_per_em {
public:
	std::string unitsPerEmAttr;
};
class attr_v_alphabetic {
public:
	std::string vAlphabeticAttr;
};
class attr_v_hanging {
public:
	std::string vHangingAttr;
};
class attr_v_ideographic {
public:
	std::string vIdeographicAttr;
};
class attr_v_mathematical {
public:
	std::string vMathematicalAttr;
};
class attr_values {
public:
	std::string valuesAttr;
};
	class attr_vert_adv_y {
	public:
		float vertAdvYAttr;
	};
	class attr_vert_origin_x {
	public:
		float vertOriginXAttr;
	};
	class attr_vert_origin_y {
	public:
		float vertOriginYAttr;
	};
	class attr_viewBox {
	public:
		std::string viewBoxAttr;
	};
	class attr_viewTarget {
	public:
		std::string viewTargetAttr;
	};
	class attr_width {
	public:
		std::string widthAttr;
	};
	class attr_x {
	public:
		std::string xAttr;
	};
	class attr_x_height {
	public:
		float xHeightAttr;
	};
	class attr_x1 {
	public:
		std::string x1Attr;
	};
	class attr_x2 {
	public:
		std::string x2Attr;
	};
	class attr_xChannelSelector {
	public:
		enum XChannel {R, G, B, A};
		XChannel xChannelSelectorAttr = A;
	};
	class attr_preserveXmlSpace {
	public:
		bool preserveXmlSpaceAttr;
	};
	class attr_y {
	public:
		std::string yAttr;
	};
	class attr_y1 {
	public:
		std::string y1Attr;
	};
	class attr_y2 {
	public:
		std::string y2Attr;
	};
	class attr_yChannelSelector {
	public:
		enum YChannel {R, G, B, A};
		YChannel yChannelSelectorAttr = A;
	};
	class attr_z {
	public:
		float zAttr;
	};
	class attr_zoomAndPan {
	public:
		bool zoomAndPanAttr = true;
	};
/*** parse functions ***/
	void parseVertAdvY(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr);
	void parseVertOriginX(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr);
	void parseVertOriginY(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr);
	void parseX(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr);
	void parseX1(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr);
	void parseX2(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr);
	void parseY(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr);
	void parseY1(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr);
	void parseY2(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr);
	void parseAlphabetic(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr);
	void parseAccentHeight(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr);
	void parseId(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr);
	void parseClass(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr);
	void parseViewTarget(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr);
	void parseXChannelSelector(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr);
	void parsePreserveXmlSpace(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr);
	void parseYChannelSelector(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr);
	void parseZ(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr);
	void parseZoomAndPan(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr);
	void parseXHeight(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr);
	void parseWidth(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr);
	void parseViewBox(std::string value, xmlDocPtr doc, xmlNodePtr element, SVGElement *elemPtr);
};
#endif