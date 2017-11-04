#version 450
#extension GL_ARB_separate_shader_objects : enable

out gl_PerVertex {
    vec4 gl_Position;
};

vec2 positions[6] = vec2[](
    vec2(-0.5,  0.5),
    vec2( 0.5,  0.5),
    vec2( 0.5, -0.5),
    vec2(-0.5,  0.5),
    vec2( 0.5, -0.5),
    vec2(-0.5, -0.5)
);

vec2 textureCoords[6] = vec2[](
    vec2(0.255, 1.00),
    vec2(0.00, 0.655),
    vec2(0.50, 0.655),
    vec2(0.00, 0.335),
    vec2(0.50, 0.335),
    vec2(0.255, 0.00)
);

void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}