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

vec4 gaussianBlur() {

    //float weight[4] = {	0.495016, 0.229743, 0.022321, 0.000428 };
    float weight[3] = { 0.38774, 0.24477, 0.06136 }; //sigma 1.0

    vec2 resolution = vec2(viewportWidth, viewportHeight);
	vec2 normalizedTexCoords = vec2(gl_FragCoord.xy / resolution);
    vec3 result = texture(emissionInput, normalizedTexCoords).rgb * weight[0];
    // retrieve data from G-buffer

    if(horizontal){
        for(int i = 1; i < weight.length(); ++i) {
            vec2 normalizedTexCoords = vec2((gl_FragCoord.xy + vec2(1 * i, 0)) / resolution);
            result += texture(emissionInput, normalizedTexCoords).rgb * weight[i];
            normalizedTexCoords = vec2((gl_FragCoord.xy - vec2(1 * i, 0)) / resolution);
            result += texture(emissionInput, normalizedTexCoords).rgb * weight[i];
        }
    } else {
        for(int i = 1; i < weight.length(); ++i) {
            vec2 normalizedTexCoords = vec2((gl_FragCoord.xy + vec2(0, 1 * i)) / resolution);
            result += texture(emissionInput, normalizedTexCoords).rgb * weight[i];
            normalizedTexCoords = vec2((gl_FragCoord.xy - vec2(0, 1 * i)) / resolution);
            result += texture(emissionInput, normalizedTexCoords).rgb * weight[i];
        }
    }
    return vec4(result, 0.0);
}

void main()
{
    pingpong = gaussianBlur();
}
