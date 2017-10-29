#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) out vec3 fragColor;

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

vec3 colors[8] = vec3[](
    vec3(1.0, 1.0, 1.0),
    vec3(1.0, 1.0, 0.0),
    vec3(1.0, 0.0, 1.0),
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 1.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0),
    vec3(0.5, 0.5, 0.5)
);

int vectorIndices[36] = int[](
    0, 1, 3,  0, 3, 2,
    2, 3, 7,  2, 7, 6, 
    3, 1, 5,  3, 5, 7,
    1, 0, 4,  1, 4, 5,
    6, 0, 2,  6, 4, 0,
    5, 6, 7,  5, 4, 6
);

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(positions[vectorIndices[gl_VertexIndex]], 1.0);
    fragColor = colors[vectorIndices[gl_VertexIndex]];
}