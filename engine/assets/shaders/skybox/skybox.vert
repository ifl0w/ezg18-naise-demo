#version 430

layout(location = 0) in vec3 position;

out vec3 TexCoords;

uniform mat4 modelMatrix; // should be a unit matrix, except if used only for rotation or scale
uniform mat4 viewProjection;

void main(){
	TexCoords = position;

	gl_Position = viewProjection * modelMatrix * vec4(position, 1.0);
}