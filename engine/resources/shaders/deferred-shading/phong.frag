#version 430

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec4 gGlow;

in vec3 vNorm;
in vec3 vPos;
in vec2 vUV;
in mat3 TBN;

uniform sampler2D colorTexture;
uniform bool useColorTexture;
uniform sampler2D specularTexture;
uniform bool useSpecularTexture;
uniform sampler2D normalTexture;
uniform bool useNormalTexture;
uniform sampler2D glowTexture;
uniform bool useGlowTexture;

layout(std140, binding = 0) uniform screenData
{
    int viewportWidth;
    int viewportHeight;
    int multiSampling;
    float brightnessFactor; // multiplied to the color to lighten or darken the result
};

uniform struct Material {
	vec3 diffuse; // kd
	vec3 specular; // ks
	vec3 ambient; // ka
	vec3 glow; // (vec3(0) = no glow)
	float shininess; // alpha but better name
} material;

uniform vec3 cameraPosition;

uniform mat4 modelMatrix;

void main() {

	vec3 ambientLighting = vec3(1); // should be sum of all ambient light intensities

    // fragment position vector
    gPosition = vec4(vPos, gl_FragCoord.z);


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
    gAlbedoSpec.rgb = material.diffuse;
    if (useColorTexture) {
        gAlbedoSpec.rgb *= texture(colorTexture, vUV).rgb; // set diffuse to 1 if direct texture is desired
    }

    // intensity in gAlbedoSpec's alpha component
    gAlbedoSpec.a = 0;
    if (useSpecularTexture) {
        gAlbedoSpec.a = texture(specularTexture, vUV).r;
    }

    // GLOW
    gGlow = vec4(material.glow, 0.0);
    if (useGlowTexture) {
        gGlow.rgb = texture(glowTexture, vUV).rgb;
        gGlow.a = 1.0;
        if(gGlow.rgb == vec3(0.0)){
            gGlow = vec4(0.0, 0.0, 0.0, 0.0);
        }
    }

    //BLOOM
    // check whether fragment output is higher than threshold, if so output as brightness color
    /*float brightness = dot(gAlbedoSpec.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 0.5)
        // gGlow = vec4(gAlbedoSpec.rgb + 1, 1.0);
        gGlow = vec4(1.0);
    else
        gGlow = vec4(0.0, 0.0, 0.0, 0.0);*/
}