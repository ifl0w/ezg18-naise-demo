#version 430
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 modelMatrix;

void main()
{
    TexCoords = aPos;
    gl_Position = modelMatrix * vec4(aPos, 1.0);
}