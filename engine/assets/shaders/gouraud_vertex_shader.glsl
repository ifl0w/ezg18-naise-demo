#version 430

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform vec3 cameraPosition;
uniform vec3 color;

uniform mat4 viewProjectionMatrix;
uniform mat4 modelMatrix;

out vec3 vPos;
out vec3 vNorm;
out vec4 vColor;

#define MAX_LIGHTS 20
uniform int countLights;
uniform struct Light {
	vec3 lightPosition;
	vec3 specular; // is
	vec3 diffuse; // id
	vec3 ambient; // ia
	float attConstant; // attenuation constant
	float attLinear; // attenuation linear
	float attQuadratic; // attenuation quadratic
	float coneAngle;
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
	vec3 spec = light.specular * material.specular * pow(max(dot(R, V), 0), material.shininess);

	diff *= attenuation;
	spec *= attenuation;

	return diff + spec;
}

void main() {
	vPos = vec3(modelMatrix * vec4(position, 1));

	mat4 normalMatrix = transpose(inverse(modelMatrix));
	vNorm = normalize(vec3(normalMatrix * vec4(normal, 1)));

	gl_Position = viewProjectionMatrix * modelMatrix * vec4(position, 1);

	vec3 ambientLighting = vec3(1);

	vec3 sum = ambientLighting * material.ambient;
	for(int i = 0; i < countLights; ++i){
		sum += processLight(lights[i], vPos, vNorm);
	}
	
	vColor = vec4(sum, 1);
}