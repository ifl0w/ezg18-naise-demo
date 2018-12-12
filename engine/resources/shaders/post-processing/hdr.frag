#version 430

in vec2 texture_coordinates;

uniform sampler2D imageInput;
uniform bool horizontal;

layout(std140, binding = 0) uniform screenData
{
	int viewportWidth;
	int viewportHeight;
	int multiSampling;
	float brightnessFactor;
};

void main() {
	gl_FragColor = vec4(1,0,1,1);//texture(imageInput, gl_FragCoord.xy);
}
