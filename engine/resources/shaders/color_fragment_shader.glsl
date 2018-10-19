#version 430

in vec3 vNorm;

layout(std140, binding = 0) uniform screenData
{
    int viewportWidth;
    int viewportHeight;
    int multiSampling;
    float brightnessFactor;
};

uniform vec3 color;

out vec4 fragColor;

void main() {
  fragColor = vec4(color*brightnessFactor, 1);
}