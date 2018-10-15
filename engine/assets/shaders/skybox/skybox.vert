#version 430

layout(location = 0) in vec3 position;

layout(std140, binding = 1) uniform projectionData
{
    mat4 viewProjection;    // size = 64B
    mat4 projectionMatrix;    // size = 64B
    mat4 viewMatrix;    // size = 64B
    vec3 cameraPosition;    // size = 16B
};

out vec3 TexCoords;

uniform mat4 modelMatrix; // should be a unit matrix, except if used only for rotation or scale

void main(){
	TexCoords = position;

	mat4 viewProjectionMatrix = projectionMatrix * mat4(mat3(viewMatrix));
	gl_Position = viewProjectionMatrix * modelMatrix * vec4(position, 1.0);
}