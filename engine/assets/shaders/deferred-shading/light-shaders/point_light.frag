#version 430 core

out vec4 fragColor;

in vec2 TexCoords;

uniform sampler2DMS gPosition;
uniform sampler2DMS gNormal;
uniform sampler2DMS gAlbedoSpec;
uniform sampler2DMS gGlow;

layout(std140, binding = 0) uniform screenData
{
    int viewportWidth;
    int viewportHeight;
    int multiSampling;
    float brightnessFactor;
};

layout(std140, binding = 1) uniform projectionData
{
    mat4 viewProjection;    // size = 64B
    vec3 cameraPosition;    // size = 16B
};

uniform struct Light {
	vec4 lightPosition;
	vec4 specular; // is
	vec4 diffuse; // id
	vec4 ambient; // ia
   	vec4 direction;
	float attConstant; // attenuation constant
	float attLinear; // attenuation linear
	float attQuadratic; // attenuation quadratic
	float coneAngle; // opening angular of the spot in degrees
	float coneBlend; // 0-1 when to start angular fall off (0 is a hard edge)
	bool directional;
} light;

layout(std430, binding = 0) buffer LightData
{
    Light lightX; // size = (5*16+6*4+8) * lights
};

uniform bool debugLightVolumes = false;

vec3 processLight(Light light, vec3 pos, vec3 norm, vec3 diffuse, float specular, vec3 glow)
{
	norm = normalize(norm);
	vec3 lightDir; // from surface to light
	float attenuation;

    //point light
    lightDir = normalize(light.lightPosition.xyz - pos);
    float dist = distance(light.lightPosition.xyz, pos);
    attenuation = 1 / (light.attConstant + light.attLinear * dist + light.attQuadratic * pow(dist, 2));

    if (light.coneAngle <= 180) {
        //spotlight restrictions (affects attenuation)
        float lightSurfaceAngle = degrees(acos(dot(-lightDir, normalize(light.direction.xyz))));

        if (lightSurfaceAngle > light.coneAngle) {
            attenuation = 0.0;
        } else {
            float innerAngular = light.coneAngle * (1 - light.coneBlend);
            float falloff = 1 - smoothstep(innerAngular, light.coneAngle, lightSurfaceAngle);
            attenuation *= falloff;
        }
    }

	vec3 R = reflect(-lightDir, norm); //2*(dot(L, vNorm))*vNorm - L;
	vec3 V = normalize(cameraPosition - pos); // calculate with viewMatrix: normalize(-vec3(viewMatrix * vec4(vPos,1)));

    float df = max(dot(lightDir, norm), 0.0);
    vec3 diff = light.diffuse.xyz * diffuse * df;
    vec3 glowTmp = light.diffuse.xyz * glow * df;

    vec3 spec = vec3(0);
    if (df > 0) { // check if front face
        spec = light.specular.xyz * specular * pow(max(dot(V, R), 0), 10); // TODO: get shineyness from material
    }

	diff *= attenuation;
	spec *= attenuation;

	return diff + spec;
}

void main()
{
    vec4 sumSubSamples = vec4(0);
    int samples = multiSampling;
    vec2 resolution = vec2(viewportWidth, viewportHeight);
    ivec2 denormalizedTexCoords = ivec2(gl_FragCoord.xy);

    for(int i = 0; i < samples; ++i)
    {
        // retrieve data from G-buffer
        vec3 FragPos = texelFetch(gPosition, denormalizedTexCoords, i).rgb;
        vec3 Normal = texelFetch(gNormal, denormalizedTexCoords, i).rgb;
        vec3 Albedo = texelFetch(gAlbedoSpec, denormalizedTexCoords, i).rgb;
        float Specular = texelFetch(gAlbedoSpec, denormalizedTexCoords, i).a;
        vec3 Glow = texelFetch(gGlow, denormalizedTexCoords, i).rgb;

        // then calculate lighting as usual
        vec3 lighting = processLight(light, FragPos, Normal, Albedo, Specular, Glow);

    	sumSubSamples += vec4(lighting, 1.0);
    }

    fragColor = sumSubSamples/samples * brightnessFactor;

    if(debugLightVolumes) {
        fragColor += vec4(0, 0.1, 0, 0);
    }

}
