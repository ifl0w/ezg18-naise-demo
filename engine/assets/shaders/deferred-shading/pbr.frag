#version 430

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoRoughness;
layout (location = 3) out vec4 gGlowMetallic;

in vec3 vNorm;
in vec3 vPos;
in vec2 vUV;
in mat3 TBN;
in vec3 Reflection;

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

uniform projectionData
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
    gPosition = vPos;

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

    // metallic in gGlowMetallic's alpha component
    gGlowMetallic.a = material.metallic;
    if (useMetallicRoughnessTexture) {
        gGlowMetallic.a = texture(metallicRoughnessTexture, vUV).r;
    }

    // roughness in gAlbedoRoughness's alpha component
    gAlbedoRoughness.a = material.roughness;
    if (useMetallicRoughnessTexture) {
        gAlbedoRoughness.a = texture(metallicRoughnessTexture, vUV).g;
    }

    // GLOW
    gGlowMetallic.rgb = material.emission;
    if (useEmissionTexture) {
        gGlowMetallic.rgb = texture(emissionTexture, vUV).rgb;
        gGlowMetallic.a = 1.0;
        if(gGlowMetallic.rgb == vec3(0.0)){
            gGlowMetallic = vec4(0.0, 0.0, 0.0, 0.0);
        }
    }

    //Cubemap Reflections (only glossy)
    if (useSkyboxTexture) {
        float roughnessFactor = gAlbedoRoughness.a;
        float metalicFactor = gGlowMetallic.a;

        int mipmapCount = textureQueryLevels(skyboxTexture);
        int mipmapLevel = int(roughnessFactor * mipmapCount);
        vec3 reflectionColor = textureLod(skyboxTexture, normalize(Reflection), mipmapLevel).rgb;

        // non-metallic materials do not reflect color
        float lighness = length(reflectionColor)/ length(vec3(1));
        // non-metallic materials only reflect brighter values
        vec3 clampedLightness = clamp(vec3(lighness), gAlbedoRoughness.rgb, vec3(1));
        vec3 m = mix(clampedLightness, reflectionColor, metalicFactor);
        // less rougher materials have more reflection and lesser diffuse color
        // in contrast to non-metallic materials, metal reflects the environment color
        vec3 r = mix(reflectionColor, gAlbedoRoughness.rgb, roughnessFactor);

        reflectionColor = (m + r) / 2;

        gAlbedoRoughness.rgb *= reflectionColor;
    }

    //BLOOM
    // check whether fragment output is higher than threshold, if so output as brightness color
    /*float brightness = dot(gAlbedoRoughness.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 0.5)
        // gGlow = vec4(gAlbedoRoughness.rgb + 1, 1.0);
        gGlow = vec4(1.0);
    else
        gGlow = vec4(0.0, 0.0, 0.0, 0.0);*/
}