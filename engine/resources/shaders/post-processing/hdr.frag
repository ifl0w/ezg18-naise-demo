#version 430

in vec2 texture_coordinates;

out vec4 fragColor;

uniform sampler2D imageInput;
layout(binding=0, rgba16f) uniform image2D adaptionRate;
layout(binding=1, rgba16f) uniform readonly image2D luminanceData;

uniform int mipmapCount;
uniform float deltaTime;
uniform vec3 mapping = vec3(1,1,1);

layout(std140, binding = 0) uniform screenData
{
	int viewportWidth;
	int viewportHeight;
	int multiSampling;
	float brightnessFactor;
};


float A = 0.15;
float B = 0.50;
float C = 0.10;
float D = 0.20;
float E = 0.02;
float F = 0.30;
float W = 11.2;

vec3 Uncharted2Tonemap(vec3 x)
{
    vec3 t = ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
    return t;
}

// https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
vec3 ACESFilm( vec3 x )
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return ((x*(a*x+b))/(x*(c*x+d)+e));
}

float adaptEye(float avgLum, float prevLum) {
    float sigma = 0.04 / (0.04 + avgLum);
    float tau = sigma * 0.4 + (1 - sigma) * 0.1;
    return prevLum + (avgLum - prevLum) * (1 - exp(-deltaTime/tau));
}

void main() {
//	fragColor = textureLod(imageInput, vec2(0.5,0.5), 0).rgba;
	vec2 resolution = vec2(viewportWidth, viewportHeight);

	int level = 0;
    ivec2 s = textureSize(imageInput, level);
    ivec2 coords = ivec2(gl_FragCoord.xy/resolution * s);
    vec3 color = texelFetch(imageInput, coords, level).rgb;

    vec4 lumData = imageLoad(luminanceData, ivec2(0,0));

    float l_av = adaptEye(exp(lumData.r), lumData.w);

    vec3 rgbFactors = vec3(0.2126, 0.7152, 0.0722);
    float l_col = rgbFactors.r * color.r + rgbFactors.g * color.g + rgbFactors.b * color.b;

    float key = 0.01;
//    key = max(0, 1.5 - (1.5/(l_av * 0.1 + 1))) + 1;

    float l_scaled = key * l_col / l_av;
//
//    vec3 scaledColor = color * l_scaled;
//
//    vec3 hdrColor = (scaledColor / (scaledColor + 1));
//
//    fragColor = vec4(hdrColor, 1);
//
    float ExposureBias = l_scaled;
    float exposure = 0.18 / l_av;
    vec3 col = ACESFilm(exposure*color);

    vec3 whiteScale = 1.0f/ACESFilm(vec3(W));
    vec3 col_white = col*whiteScale;
    fragColor = vec4(col_white, 1);

    imageStore(adaptionRate, coords, vec4(l_av));
}
