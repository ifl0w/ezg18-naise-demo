#version 430

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoRoughness;
layout (location = 3) out vec4 gGlowMetallic;

in vec3 vNorm;
in vec3 vPos;
in vec2 vUV;
in mat3 TBN;

uniform sampler2D albedoTexture;
uniform bool useAlbedoTexture;
uniform sampler2D roughnessTexture;
uniform bool useRoughnessTexture;
uniform sampler2D metallicTexture;
uniform bool useMetallicTexture;
uniform sampler2D normalTexture;
uniform bool useNormalTexture;
uniform sampler2D emissionTexture;
uniform bool useEmissionTexture;

layout(std140, binding = 0) uniform screenData
{
    int viewportWidth;
    int viewportHeight;
    int multiSampling;
    float brightnessFactor; // multiplied to the color to lighten or darken the result
};

uniform struct Material {
	vec3 albedo;
	float roughness;
	float metallic;
	vec3 emission; // (vec3(0) = no glow)
} material;

uniform vec3 cameraPosition;

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

    // roughness in gAlbedoRoughness's alpha component
    gAlbedoRoughness.a = material.roughness;
    if (useRoughnessTexture) {
        gAlbedoRoughness.a = texture(roughnessTexture, vUV).r;
    }

    // metallic in gGlowMetallic's alpha component
    gGlowMetallic.a = material.metallic;
    if (useEmissionTexture) {
        gGlowMetallic.a = texture(metallicTexture, vUV).r;
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

    //BLOOM
    // check whether fragment output is higher than threshold, if so output as brightness color
    /*float brightness = dot(gAlbedoRoughness.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 0.5)
        // gGlow = vec4(gAlbedoRoughness.rgb + 1, 1.0);
        gGlow = vec4(1.0);
    else
        gGlow = vec4(0.0, 0.0, 0.0, 0.0);*/
}