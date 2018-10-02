#version 430

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

layout(std140, binding = 1) uniform projectionData
{
    mat4 viewProjection;    // size = 64B
    vec3 cameraPosition;    // size = 16B
};

uniform mat4 modelMatrix;

out vec3 vPos;
out vec3 vNorm;
out vec2 vUV;

void main() {
	vPos = vec3(modelMatrix * vec4(position, 1));
	vUV = uv;

	mat4 normalMatrix = transpose(inverse(modelMatrix));
	vNorm = normalize(vec3(normalMatrix * vec4(normal, 1)));

	gl_Position = viewProjection * modelMatrix * vec4(position, 1);
}