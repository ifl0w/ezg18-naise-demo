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

    // simple hdr from learnopengl.com
    // TODO: has to be replaced
    vec3 hdrColor = color;
    float exposure = 2;
    // Exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
    // Gamma correction

    fragColor = vec4(mapped, 1);
}
