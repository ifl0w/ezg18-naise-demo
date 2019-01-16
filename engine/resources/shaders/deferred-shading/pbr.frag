#version 430

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoRoughness;
layout (location = 3) out vec4 gEmissionMetallic;
layout (location = 4) out float gLinearDepth;

in vec3 vNorm;
in vec3 vPos;
in vec2 vUV;
in mat3 TBN;
in vec3 Reflection;
in float LinearDepth;

uniform sampler2D albedoTexture;
uniform bool useAlbedoTexture;
uniform sampler2D metallicRoughnessTexture; // packed R = metallic G = roughness
uniform bool useMetallicRoughnessTexture;
uniform sampler2D normalTexture;
uniform bool useNormalTexture;
uniform sampler2D emissionTexture;
uniform bool useEmissionTexture;
uniform samplerCube skyboxTexture;
uniform bool useSkyboxTexture;

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

uniform struct Material {
	vec3 albedo;
	float roughness;
	float metallic;
	vec3 emission; // (vec3(0) = no glow)
} material;

//uniform vec3 cameraPosition;

uniform mat4 modelMatrix;

void main() {

	vec3 ambientLighting = vec3(1); // should be sum of all ambient light intensities

    // fragment position vector
    gPosition = vec4(vPos, gl_FragCoord.z);
    gPosition.a = LinearDepth;
    //gLinearDepth = LinearDepth;
    gLinearDepth = gl_FragCoord.z;

    // per-fragment normals
    gNormal = normalize(vNorm);
    if (useNormalTexture) {
        // transform normalMap from range [0, 1] to [-1, 1] and normalize
        gNormal = texture(normalTexture, vUV).rgb;

        gNormal = normalize(gNormal * 2.0 - 1.0);
        // tangent-space to world-space
        gNormal = normalize(TBN * gNormal);
    }

    // diffuse per-fragment color
    gAlbedoRoughness.rgb = material.albedo;
    if (useAlbedoTexture) {
        gAlbedoRoughness.rgb *= texture(albedoTexture, vUV).rgb; // set diffuse to 1 if direct texture is desired
    }

    // metallic in gEmissionMetallic's alpha component
    gEmissionMetallic.a = material.metallic;
    if (useMetallicRoughnessTexture) {
        gEmissionMetallic.a = texture(metallicRoughnessTexture, vUV).r;
    }

    // roughness in gAlbedoRoughness's alpha component
    gAlbedoRoughness.a = material.roughness;
    if (useMetallicRoughnessTexture) {
        gAlbedoRoughness.a = texture(metallicRoughnessTexture, vUV).g;
    }

    // GLOW
    gEmissionMetallic.rgb = material.emission;
    if (useEmissionTexture) {
        gEmissionMetallic.rgb = texture(emissionTexture, vUV).rgb;
    }

    //BLOOM in new pass after everything is rendered
    // check whether fragment output is higher than threshold, if so output as brightness color
    /*float brightness = dot(gAlbedoRoughness.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 0.5)
        // gGlow = vec4(gAlbedoRoughness.rgb + 1, 1.0);
        gGlow = vec4(1.0);
    else
        gGlow = vec4(0.0, 0.0, 0.0, 0.0);*/


    //Cubemap Reflections (only glossy)
    if (useSkyboxTexture) { //false &&
        float roughnessFactor = gAlbedoRoughness.a;
        float metalicFactor = gEmissionMetallic.a;

        int mipmapCount = textureQueryLevels(skyboxTexture);
        int mipmapLevel = int(roughnessFactor * mipmapCount);
        vec3 reflectionColor = textureLod(skyboxTexture, normalize(Reflection), mipmapLevel).rgb;

        // non-metallic materials do not reflect color
        float lighness = length(reflectionColor)/ length(vec3(1));
        // non-metallic materials only reflect brighter values
        vec3 clampedLightness = clamp(vec3(lighness), gAlbedoRoughness.rgb, vec3(1));
        // less rougher materials have more reflection and lesser diffuse color
        // in contrast to non-metallic materials, metal reflects the environment color
        reflectionColor  = mix(reflectionColor, gAlbedoRoughness.rgb, roughnessFactor);
        reflectionColor = mix(clampedLightness, reflectionColor, metalicFactor);

        gAlbedoRoughness.rgb *= reflectionColor;
    }
}