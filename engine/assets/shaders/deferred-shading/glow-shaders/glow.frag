#version 430

layout (location = 0) out vec4 pingpong;

in vec2 TexCoords;

uniform sampler2D emissionInput;
uniform bool horizontal;

layout(std140, binding = 0) uniform screenData
{
	int viewportWidth;
	int viewportHeight;
	int multiSampling;
	float brightnessFactor;
};

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
	vec2 normalizedTexCoords = vec2(gl_FragCoord.xy / resolution);
    vec3 result = texture(emissionInput, normalizedTexCoords).rgb * weight[0];
    // retrieve data from G-buffer

    if(horizontal){
        for(int i = 1; i < newKernelSize; ++i) {
            result += texture(emissionInput, vec2(normalizedTexCoords.x + 1 * i, normalizedTexCoords.y)).rgb * weight[i];
            result += texture(emissionInput, vec2(normalizedTexCoords.x - 1 * i, normalizedTexCoords.y)).rgb * weight[i];
        }
    } else {
        for(int i = 1; i < newKernelSize; ++i) {
            result += texture(emissionInput, vec2(normalizedTexCoords.x, normalizedTexCoords.y + 1 * i)).rgb * weight[i];
            result += texture(emissionInput, vec2(normalizedTexCoords.x, normalizedTexCoords.y - 1 * i)).rgb * weight[i];
        }
    }
    return vec4(result, 0.0);
}

void main()
{
    pingpong = gaussianBlur();
}
