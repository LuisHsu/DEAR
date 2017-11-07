#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform sampler2D texSampler;
layout(location = 0) out vec4 outColor;
layout(location = 0) in vec2 textureCoord;

void main() {
    vec4 tcolor = texture(texSampler, textureCoord);
    tcolor.w = 0.5;
    outColor = tcolor;
}
