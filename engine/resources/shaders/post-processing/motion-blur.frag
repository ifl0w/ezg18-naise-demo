#version 430

out vec4 fragColor;
in vec2 texture_coordinates;

layout(binding=0, rgba32f) uniform readonly image2D currentPosition;

//layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

uniform sampler2D inputImage;

uniform float shutterSpeed = 100; // milliseconds
uniform float deltaTime;
uniform mat4 previousViewMatrix;
uniform mat4 previousProjectionMatrix;

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

vec4 toScreenSpace(vec4 position, mat4 matrix) {
    vec4 result = matrix * position;
    result.xyz /= result.w;
    // NDC -> (0,1) so it can be used as TexCoords
    result.xyz = result.xyz * 0.5 + 0.5;

    return result;
}

void main() {
    vec2 resolution = vec2(viewportWidth, viewportHeight);
    ivec2 coords = ivec2(gl_FragCoord.xy);

    vec4 currPos = imageLoad(currentPosition, coords);
    vec4 oldPos = currPos;

    vec4 screen_oldPos = toScreenSpace(vec4(oldPos.xyz, 1), previousProjectionMatrix * previousViewMatrix);
    vec4 screen_currPos = toScreenSpace(vec4(currPos.xyz, 1), projectionMatrix * viewMatrix);

    vec2 posDiff = (screen_oldPos.xy - screen_currPos.xy) * deltaTime * shutterSpeed;

    int iterations = 64;
    vec2 sampleStep = posDiff.xy / float(iterations);
    vec2 samplePosition = (coords + vec2(0.5,0.5))/resolution;

    vec4 color = texture(inputImage, samplePosition);

    for (int i = 0; i < iterations; i++) {
        sampleStep = posDiff.xy * ( float(i) / float(iterations) - 0.5);
        color += texture(inputImage, samplePosition + sampleStep);
    }

    color /= float(iterations);

    fragColor = color;
}
