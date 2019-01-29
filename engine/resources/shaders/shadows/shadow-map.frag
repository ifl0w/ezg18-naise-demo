#version 430

layout (location = 0) out float shadowmap;

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

void main() {
    // fragment position vector
    shadowmap = gl_FragCoord.z;
}