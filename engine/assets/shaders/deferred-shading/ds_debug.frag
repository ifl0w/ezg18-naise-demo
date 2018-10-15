#version 430 core

out vec4 fragColor;

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

vec4 renderMultisampled() {
    vec4 sumSubSamples = vec4(0);
    int samples = multiSampling;
    vec2 resolution = vec2(viewportWidth, viewportHeight);
    ivec2 denormalizedTexCoords = ivec2(TexCoords * resolution);

    for(int i = 0; i < samples; ++i)
    {
        vec3 debugSample = texelFetch(debugTextureMS, denormalizedTexCoords, i).rgb;
    	sumSubSamples += vec4(debugSample, 1.0);
    }

    return sumSubSamples/samples;
}

void main()
{
    if (isMultiSampled) {
        fragColor = renderMultisampled();
    } else {
        fragColor = texture(debugTexture, TexCoords);
    }
}
