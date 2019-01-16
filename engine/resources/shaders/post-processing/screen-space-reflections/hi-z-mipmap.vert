#version 430

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

uniform mat4 modelMatrix;

out vec2 TexCoords;

void main() {
    TexCoords = uv;
    gl_Position = modelMatrix * vec4(position, 1);
}