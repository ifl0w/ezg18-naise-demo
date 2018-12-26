#version 430 core

#define PI 3.1415926535897932384626433832795

out vec4 fragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoRoughness;
uniform sampler2D gEmissionMetallic;

uniform sampler2DShadow shadowMap[3];
uniform mat4 depthShadowProjection[3];
uniform float cascadeEnd[3];

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

float calculateShadowFactor(vec3 pos, vec3 normal, vec3 lightDir) {
    int cascadeIdx = 0;
    vec4 proj = viewProjection * vec4(pos, 1);
    float depth = proj.z / proj.w;

    for (int i = 0 ; i < 3 ; i++) {
        if (depth <= cascadeEnd[i]) {
            cascadeIdx = i;
            break;
        }
    }

	vec4 shadowPos = (depthShadowProjection[cascadeIdx]) * vec4(pos,1);
	vec3 ProjCoords =  0.5 * (shadowPos.xyz) + 0.5;

	float cosTheta = clamp(dot(normal, lightDir), 0.0, 1.0);
    float bias = 0.0001 * tan(acos(cosTheta)) * pow(10,cascadeIdx);
    bias = clamp(bias, 0.0, 0.01);

    // 2x2 hardware pcf + blurring with a 3x3 kernel for prettier results
    float shadowFactor = 0;
    for(int x=-1; x<=1; x++) {
        for(int y=-1; y<=1; y++) {
            vec2 off = vec2(x,y)/3000; // blurring factor depends on cascade size
            shadowFactor += texture(shadowMap[cascadeIdx], vec3(ProjCoords.xy+off, ProjCoords.z-bias));
        }
    }
    shadowFactor /= 9;

	return shadowFactor;
}

//vec3 processLight(Light light, vec3 pos, vec3 norm, vec3 diffuse, float specular)
//{
//	norm = normalize(norm);
//	vec3 lightDir; // from surface to light
//	float attenuation;
//
//	//directional light
//	lightDir = normalize(-light.direction.xyz);
//	attenuation = 1.0; // no attenuation for directional lights
//
//	vec3 R = reflect(-lightDir, norm); //2*(dot(L, vNorm))*vNorm - L;
//	vec3 V = normalize(cameraPosition - pos); // calculate with viewMatrix: normalize(-vec3(viewMatrix * vec4(vPos,1)));
//
//	float df = max(dot(lightDir, norm), 0.0);
//	vec3 diff = light.diffuse.xyz * diffuse * df;
//
//	vec3 spec = vec3(0);
//	if (df > 0) { // check if front face
//		spec = light.specular.xyz * specular * pow(max(dot(V, R), 0), 10); // TODO: get shineyness from material
//	}
//
//	diff *= attenuation;
//	spec *= attenuation;
//
//	float shadowFactor = calculateShadowFactor(pos, norm, lightDir);
//	return (diff + spec) * shadowFactor;
//}

vec3 fresnelSchlick(float cosTheta, vec3 f0)
{
    return f0 + (1.0 - f0) * pow(max(1.0 - cosTheta, 0), 5.0);
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

    return num / max(denom, 0.0001);
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
	vec3 toLight = normalize(-light.direction.xyz);
	vec3 toCam = normalize(cameraPosition - pos);

	float attenuation = 1;
	float intensityFactor = 1;

	vec3 halfVec = normalize(toCam + toLight);

	vec3 f0 = vec3(0.04);
	f0 = mix(f0, albedo, metallic);
	vec3 f = fresnelSchlick(max(dot(halfVec, toCam), 0.0), f0);

//	float NDF = DistributionGGX(norm, halfVec, roughness);
	float NDF = DistributionBeckmann(norm, halfVec, roughness);
	float G = GeometrySmith(norm, toCam, toLight, roughness);
//	float G = geomAttinuationWikipedia(halfVec, norm, toCam, toLight);

	vec3 numerator = NDF * G * f;
	float denominator = 4.0 * max(dot(toCam, norm), 0.0) * max(dot(norm, toLight), 0.0);
	vec3 specular = numerator / max(denominator, 0.0001);

	vec3 kD = vec3(1.0) - specular;
	kD *= 1.0 - metallic;

	vec3 radiance = (light.diffuse.xyz * 1 / attenuation) * intensityFactor;

    float NdotL = max(dot(norm, toLight), 0.0);

    vec3 color = (kD * albedo / PI + specular) * radiance * NdotL;
    float shadowFactor = calculateShadowFactor(pos, norm, toLight);

    // code for cascade visualization
//    vec4 proj = viewProjection * vec4(pos, 1);
//    float depth = proj.z / proj.w;
//    if (depth <= cascadeEnd[0]) {
//        color = vec3(1,0,0);
//    } else if(depth <= cascadeEnd[1]) {
//        color = vec3(0,1,0);
//    } else if(depth <= cascadeEnd[2]) {
//        color = vec3(0,0,1);
//    }

    return color * shadowFactor;
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

    lighting += processLight(light, FragPos, Normal, Albedo, Roughness, Metallic);

	fragColor = vec4(lighting, 1) * brightnessFactor;
}
