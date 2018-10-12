#version 430

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

layout(std140, binding = 1) uniform projectionData
{
    mat4 viewProjection;    // size = 64B
    mat4 projectionMatrix;    // size = 64B
    mat4 viewMatrix;    // size = 64B
    vec3 cameraPosition;    // size = 16B
};

uniform mat4 modelMatrix;

out vec2 TexCoords;

void main() {
    TexCoords = uv;

    gl_Position = viewProjection * modelMatrix * vec4(position, 1);
}