#version 430

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

uniform bool useInstancing = false;
layout(std430, binding = 0) buffer InstanceTransforms
{
    mat4 instanceTransforms[];
};

layout(std140, binding = 1) uniform projectionData
{
    mat4 viewProjection;    // size = 64B
    mat4 projectionMatrix;    // size = 64B
    mat4 viewMatrix;    // size = 64B
    vec3 cameraPosition;    // size = 16B
};

uniform mat4 modelMatrix;

out vec3 vPos;
out vec3 vNorm;
out vec2 vUV;

void main() {
    mat4 mMatrix = modelMatrix;

    if(useInstancing) {
        mMatrix = instanceTransforms[gl_InstanceID];
    }

	vPos = vec3(mMatrix * vec4(position, 1));
	vUV = uv;

	mat4 normalMatrix = transpose(inverse(mMatrix));
	vNorm = normalize(vec3(normalMatrix * vec4(normal, 1)));

	gl_Position = viewProjection * mMatrix * vec4(position, 1);
}