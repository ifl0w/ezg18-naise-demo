#version 430

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec3 vNorm;
in vec3 vPos;
in vec2 vUV;

layout(std140, binding = 0) uniform screenData
{
    int viewportWidth;
    int viewportHeight;
    int multiSampling;
    float brightnessFactor; // multiplied to the color to lighten or darken the result
};

layout(std140, binding = 1) uniform projectionData
{
    mat4 viewProjection;    // size = 64B
    mat4 projectionMatrix;    // size = 64B
    mat4 viewMatrix;    // size = 64B
    vec3 cameraPosition;    // size = 16B
};

#define MAX_LIGHTS 90
uniform int countLights;
uniform struct Light {
	vec3 lightPosition;
	vec3 specular; // is
	vec3 diffuse; // id
	vec3 ambient; // ia
	float attConstant; // attenuation constant
	float attLinear; // attenuation linear
	float attQuadratic; // attenuation quadratic
	float coneAngle; // opening angular of the spot in degrees
	float coneBlend; // 0-1 when to start angular fall off (0 is a hard edge) 
   	vec3 direction;
	bool directional;
} lights[MAX_LIGHTS];

uniform struct Material {
	vec3 diffuse; // kd
	vec3 specular; // ks
	vec3 ambient; // ka
	float shininess; // alpha but better name
} material;

uniform sampler2D colorTexture;
uniform bool useColorTexture;
uniform sampler2D specularTexture;
uniform bool useSpecularTexture;

out vec4 fragColor;

vec3 processLight(Light light, vec3 pos, vec3 norm)
{
	norm = normalize(norm);
	vec3 L; // from surface to light
	float attenuation;

	if(light.directional) {
        //directional light
        L = normalize(-light.direction);
        attenuation = 1.0; // no attenuation for directional lights
    } else {
        //point light
        L = normalize(light.lightPosition - pos);
		float dist = distance(light.lightPosition, pos);
		attenuation = 1 / (light.attConstant + light.attLinear * dist + light.attQuadratic * pow(dist, 2));

		if (light.coneAngle <= 180) {
        	//spotlight restrictions (affects attenuation)
			float lightSurfaceAngle = degrees(acos(dot(-L, normalize(light.direction))));

			if (lightSurfaceAngle > light.coneAngle) {
				attenuation = 0.0;
			} else {
				float innerAngular = light.coneAngle * (1 - light.coneBlend);
				float falloff = 1 - smoothstep(innerAngular, light.coneAngle, lightSurfaceAngle);
				attenuation *= falloff;
			}
		}
    }

	vec3 R = reflect(-L, norm); //2*(dot(L, vNorm))*vNorm - L;
	vec3 V = normalize(cameraPosition - pos); // calculate with viewMatrix: normalize(-vec3(viewMatrix * vec4(vPos,1)));

	vec3 diff = light.diffuse * material.diffuse * max(dot(L, norm), 0.0);
	if (useColorTexture) {
		diff *= texture(colorTexture, vUV).xyz; // set diffuse to 1 if direct texture is desired
	}

	vec3 spec = light.specular * material.specular * pow(max(dot(R, V), 0), material.shininess);
	if (useSpecularTexture) {
		spec *= texture(specularTexture, vUV).xyz;
	}

	diff *= attenuation;
	spec *= attenuation;

	return diff + spec;
}

void main() {
	vec3 ambientLighting = vec3(1); // should be sum of all ambient light intensities

	vec3 sum = ambientLighting * material.ambient;
	if (useColorTexture) {
		sum *= texture(colorTexture, vUV).xyz;
	}

	for(int i = 0; i < countLights; ++i){
		sum += processLight(lights[i], vPos, vNorm);
	}

	fragColor = vec4(sum * brightnessFactor, 1);
}