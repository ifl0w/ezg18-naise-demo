#version 430

uniform float brightnessFactor; // multiplied to the color to lighten or darken the result

uniform vec3 cameraPosition;

in vec3 vNorm;
in vec3 vPos;
in vec4 vColor;

struct PointLightInfo {
	vec3 position;
	vec3 is; // specular
	vec3 id; // diffuse
	vec3 ia; // ambient
};

//uniform PointLight pointLight;
uniform vec3 color;

out vec4 diffuseColor;

void main() {
	diffuseColor = vec4(vColor.xyz * brightnessFactor, 1);
}