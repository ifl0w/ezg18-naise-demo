#version 430 core

#define PI 3.1415926535897932384626433832795

out vec4 fragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoRoughness;
uniform sampler2D gEmissionMetallic;

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
    mat4 projectionMatrix;    // size = 64B
    mat4 viewMatrix;    // size = 64B
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

vec3 fresnelSchlick(float cosTheta, vec3 f0)
{
    return f0 + (1.0 - f0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float DistributionBeckmann(vec3 N, vec3 H, float roughness)
{
	float m2 = max(roughness * roughness, 0.0001);
    float NdotH = max(dot(N, H), 0.0001);
    float cos2Alpha = NdotH*NdotH;
    float tan2Alpha = (1 - cos2Alpha) / cos2Alpha;

	float num = exp(-tan2Alpha/m2);
	float denom = PI * m2 * cos2Alpha * cos2Alpha;

	return num/denom;
}

float geomAttinuationWikipedia(vec3 H, vec3 N, vec3 V, vec3 L) {
	float VdotH = max(dot(V,H), 0.0001);
	float HdotN = max(dot(H,N), 0);
	float VdotN = max(dot(V,N), 0);
	float LdotN = max(dot(L,N), 0);
	float t1 = 2*(HdotN*VdotN) / VdotH;
	float t2 = 2*(HdotN*LdotN) / VdotH;
	return min(1, min(t1,t2));
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 processLight(Light light, vec3 pos, vec3 norm, vec3 albedo, float roughness, float metallic) {
	vec3 toLight;
	vec3 toCam = normalize(cameraPosition - pos);

	float attenuation = 1;
	float intensityFactor = 1;

    toLight = normalize(light.lightPosition.xyz - pos);

    float dist = distance(pos, light.lightPosition.xyz);
    attenuation = light.attLinear + dist * light.attQuadratic + pow(dist, 2) * light.attConstant;

    if (light.coneAngle < 180) { // spotlight
        float cosPointAndLight = abs(acos(dot(light.direction.xyz, -toLight))); // should be normalized
        float cosLightOuter = abs(radians(light.coneAngle/2));
        float cosLightInner = abs(radians((light.coneAngle*(1 - light.coneBlend))/2));

        if (cosPointAndLight <= cosLightOuter) {
            intensityFactor = 1 - smoothstep(cosLightInner, cosLightOuter, cosPointAndLight);
        } else {
            intensityFactor = 0;
        }
    }

	vec3 halfVec = normalize(toCam + toLight);

	vec3 f0 = vec3(0.04);
	f0 = mix(f0, albedo, metallic);
	vec3 f = fresnelSchlick(max(dot(halfVec, toCam), 0.0), f0);

	//float NDF = DistributionGGX(norm, halfVec, roughness);
	float NDF = DistributionBeckmann(norm, halfVec, roughness);
	float G = GeometrySmith(norm, toCam, toLight, roughness);
	//float G = geomAttinuationWikipedia(halfVec, norm, toCam, toLight);

	vec3 numerator = NDF * G * f;
	float denominator = 4.0 * max(dot(toCam, norm), 0.0) * max(dot(norm, toLight), 0.0);
	vec3 specular = numerator / max(denominator, 0.0001);

	vec3 kD = vec3(1.0) - specular;
	kD *= 1.0 - metallic;

	vec3 radiance = (light.diffuse.xyz * 1 / attenuation) * intensityFactor;

    float NdotL = max(dot(norm, toLight), 0.0);
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

void main()
{
    vec2 resolution = vec2(viewportWidth, viewportHeight);
    vec2 normalizedTexCoords = vec2(gl_FragCoord.xy / resolution);

    // retrieve data from G-buffer
    vec3 FragPos = texture(gPosition, normalizedTexCoords).rgb;
    vec3 Normal = normalize(texture(gNormal, normalizedTexCoords).rgb);
    vec3 Albedo = texture(gAlbedoRoughness, normalizedTexCoords).rgb;
    float Roughness = texture(gAlbedoRoughness, normalizedTexCoords).a;
    float Metallic = texture(gEmissionMetallic, normalizedTexCoords).a;

    // then calculate lighting as usual
    vec3 lighting = Albedo * light.ambient.rgb;
    vec3 viewDir = normalize(cameraPosition - FragPos);

    lighting = processLight(light, FragPos, Normal, Albedo, Roughness, Metallic);

	fragColor = vec4(lighting, 1) * brightnessFactor;

    if(debugLightVolumes) {
        fragColor += vec4(0, 0.1, 0, 0);
    }

}
