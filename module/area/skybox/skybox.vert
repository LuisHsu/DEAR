#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) out vec2 textureCoord;

out gl_PerVertex {
    vec4 gl_Position;
};

vec3 positions[8] = vec3[](
    vec3(-2.0,  2.0, -2.0),
    vec3( 2.0,  2.0, -2.0),
    vec3(-2.0,  2.0,  2.0),
    vec3( 2.0,  2.0,  2.0),
    vec3(-2.0, -2.0, -2.0),
    vec3( 2.0, -2.0, -2.0),
    vec3(-2.0, -2.0,  2.0),
    vec3( 2.0, -2.0,  2.0)
);

vec2 textureCoords[14] = vec2[](
    vec2(0.255, 1.00), vec2(0.495, 1.00),
    vec2(0.00, 0.655), vec2(0.25, 0.655), vec2(0.50, 0.655), vec2(0.75, 0.655), vec2(1.00, 0.655),
    vec2(0.00, 0.335), vec2(0.25, 0.335), vec2(0.50, 0.335), vec2(0.75, 0.335), vec2(1.00, 0.335),
    vec2(0.255, 0.00), vec2(0.495, 0.00)
);

int vectorIndices[36] = int[](
    0, 1, 3,  0, 3, 2,
    2, 3, 7,  2, 7, 6, 
    3, 1, 5,  3, 5, 7,
    1, 0, 4,  1, 4, 5,
    6, 0, 2,  6, 4, 0,
    5, 6, 7,  5, 4, 6
);

int textureIndices[36] = int[](
    3,  4,  1,  3,  1,  0,
    6,  5, 10,  6, 10, 11, 
    5,  4,  9,  5,  9, 10,
    4,  3,  8,  4,  8,  9,
    7,  3,  2,  7,  8,  3,
    9, 12, 13,  9,  8, 12
);

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(positions[vectorIndices[gl_VertexIndex]], 1.0);
    textureCoord = textureCoords[textureIndices[gl_VertexIndex]];
}