#version 430 core

//out vec4 fragColor;

layout (location = 0) out vec4 pingpong;

in vec2 TexCoords;

uniform bool isMultiSampled;

uniform sampler2D debugTexture;
uniform sampler2DMS debugTextureMS;

layout(std140, binding = 0) uniform screenData
{
    int viewportWidth;
    int viewportHeight;
    int multiSampling;
    float brightnessFactor;
};

uniform bool horizontal;

//uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
//uniform float weight[11] = float[] (0.0093,	0.028002,	0.065984,	0.121703,	0.175713,	0.198596,	0.175713,	0.121703,	0.065984,	0.028002,	0.0093);

vec4 gaussianBlur() {
    // kernel size = 7, sigma = 0.75
    int newKernelSize = 4;
    float weight[4] = {	0.495016, 0.229743, 0.022321, 0.000428 };

/*
    int newKernelSize = 6;
    float weight[6] = {0.141836, 0.13424, 0.113806, 0.086425, 0.05879, 0.035822};
    int newKernelSize = 11;
    float weight[11] = {0.132429, 0.125337, 0.106259, 0.080693, 0.054891, 0.033446, 0.018255, 0.008925, 0.003908, 0.001533, 0.000539};
*/

    vec2 resolution = vec2(viewportWidth, viewportHeight);
    ivec2 denormalizedTexCoords = ivec2(TexCoords * resolution);
	ivec2 tmpdenormalizedTexCoords = denormalizedTexCoords;
    vec3 result = texelFetch(debugTextureMS, denormalizedTexCoords, 0).rgb * weight[0]; // current fragment's contribution * weight[0]

    if(horizontal){
        for(int i = 1; i < newKernelSize; ++i)
        {
            tmpdenormalizedTexCoords = denormalizedTexCoords;
            tmpdenormalizedTexCoords.x = tmpdenormalizedTexCoords.x + 1 * i;
            result += texelFetch(debugTextureMS, tmpdenormalizedTexCoords, 0).rgb * weight[i];

            tmpdenormalizedTexCoords = denormalizedTexCoords;
            tmpdenormalizedTexCoords.x = tmpdenormalizedTexCoords.x - 1 * i;
            result += texelFetch(debugTextureMS, tmpdenormalizedTexCoords, 0).rgb * weight[i];
        }
    } else {
        for(int i = 1; i < newKernelSize; ++i)
        {
            tmpdenormalizedTexCoords = denormalizedTexCoords;
            tmpdenormalizedTexCoords.y = tmpdenormalizedTexCoords.y + 1 * i;
            result += texelFetch(debugTextureMS, tmpdenormalizedTexCoords, 0).rgb * weight[i];

            tmpdenormalizedTexCoords = denormalizedTexCoords;
            tmpdenormalizedTexCoords.y = tmpdenormalizedTexCoords.y - 1 * i;
            result += texelFetch(debugTextureMS, tmpdenormalizedTexCoords, 0).rgb * weight[i];
        }
    }
    return vec4(result, debugTextureMS.a);
}

void main()
{
    pingpong = gaussianBlur();
}
