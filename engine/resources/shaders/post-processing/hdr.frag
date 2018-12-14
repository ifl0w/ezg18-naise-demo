#version 430

in vec2 texture_coordinates;

out vec4 fragColor;

uniform sampler2D imageInput;

uniform int mipmapCount;
uniform vec3 mapping = vec3(1,1,1);

layout(std140, binding = 0) uniform screenData
{
	int viewportWidth;
	int viewportHeight;
	int multiSampling;
	float brightnessFactor;
};

void main() {
//	fragColor = textureLod(imageInput, vec2(0.5,0.5), 0).rgba;
	vec2 resolution = vec2(viewportWidth, viewportHeight);

	int level = 0;
    ivec2 s = textureSize(imageInput, level);
    vec3 color = texelFetch(imageInput, ivec2(gl_FragCoord.xy/resolution * s) , level).rgb;
    vec3 avgCol = texelFetch(imageInput, ivec2(0,0), mipmapCount).rgb;


    float key = 1;

    float l_av = (avgCol.r + avgCol.g + avgCol.b) / 3;
    float l_col = (color.r + color.g + color.b) / 3;

    float l_scaled = key * l_col / l_av;

    vec3 scaledColor = color * l_scaled;

    vec3 hdrColor = (scaledColor / (scaledColor + 1));

    fragColor = vec4(hdrColor, 1);
}
