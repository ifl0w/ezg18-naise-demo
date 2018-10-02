#version 430 core

out vec4 fragColor;

in vec2 TexCoords;

uniform sampler2DMS gPosition;
uniform sampler2DMS gNormal;
uniform sampler2DMS gAlbedoSpec;
uniform sampler2DMS gGlow;

uniform sampler2DShadow shadowMap;
uniform mat4 depthShadowProjection;

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

float calculateShadowFactor(vec3 pos, vec3 normal, vec3 lightDir) {
	vec4 shadowPos = (depthShadowProjection) * vec4(pos,1);
	vec3 ProjCoords =  0.5 * (shadowPos.xyz) + 0.5;

	float cosTheta = clamp(dot(normal, lightDir), 0.0, 1.0);
    float bias = 0.00005 * tan(acos(cosTheta));
    bias = clamp(bias, 0.0, 0.01);

    // 2x2 hardware pcf + blurring with a 3x3 kernel for prettier results
    float shadowFactor = 0;
    for(int x=-1; x<=1; x++) {
        for(int y=-1; y<=1; y++) {
            vec2 off = vec2(x,y)/3000; // blurring factor depends on cascade size
            shadowFactor += texture(shadowMap, vec3(ProjCoords.xy+off, ProjCoords.z-bias));
        }
    }
    shadowFactor /= 9;

	return shadowFactor;
}

vec3 processLight(Light light, vec3 pos, vec3 norm, vec3 diffuse, float specular)
{
	norm = normalize(norm);
	vec3 lightDir; // from surface to light
	float attenuation;

	//directional light
	lightDir = normalize(-light.direction.xyz);
	attenuation = 1.0; // no attenuation for directional lights

	vec3 R = reflect(-lightDir, norm); //2*(dot(L, vNorm))*vNorm - L;
	vec3 V = normalize(cameraPosition - pos); // calculate with viewMatrix: normalize(-vec3(viewMatrix * vec4(vPos,1)));

	float df = max(dot(lightDir, norm), 0.0);
	vec3 diff = light.diffuse.xyz * diffuse * df;

	vec3 spec = vec3(0);
	if (df > 0) { // check if front face
		spec = light.specular.xyz * specular * pow(max(dot(V, R), 0), 10); // TODO: get shineyness from material
	}

	diff *= attenuation;
	spec *= attenuation;

	float shadowFactor = 1;//calculateShadowFactor(pos, norm, lightDir);
	return (diff + spec) * shadowFactor;
}

void main()
{
	vec4 sumSubSamples = vec4(0);
	int samples = multiSampling;
	vec2 resolution = vec2(viewportWidth, viewportHeight);
	ivec2 denormalizedTexCoords = ivec2(TexCoords * resolution);

	for(int i = 0; i < samples; ++i)
	{
		// retrieve data from G-buffer
		vec3 FragPos = texelFetch(gPosition, denormalizedTexCoords, i).rgb;
		vec3 Normal = texelFetch(gNormal, denormalizedTexCoords, i).rgb;
		vec3 Albedo = texelFetch(gAlbedoSpec, denormalizedTexCoords, i).rgb;
		float Specular = texelFetch(gAlbedoSpec, denormalizedTexCoords, i).a;

		// then calculate lighting as usual
		vec3 lighting = Albedo * light.ambient.rgb;
		vec3 viewDir = normalize(cameraPosition - FragPos);

		lighting += processLight(light, FragPos, Normal, Albedo, Specular);

		sumSubSamples += vec4(lighting, 1.0);
	}

	fragColor = sumSubSamples/samples * brightnessFactor;
}
