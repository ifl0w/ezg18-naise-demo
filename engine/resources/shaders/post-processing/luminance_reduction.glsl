#version 430

layout(binding=0, rgba16f) uniform image2D inputImage;

uniform int iteration = 0;

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(std140, binding = 0) uniform screenData
{
	int viewportWidth;
	int viewportHeight;
	int multiSampling;
	float brightnessFactor;
};

void main() {
    vec2 base = gl_GlobalInvocationID.xy;

    vec2 access00 = (base + vec2(0,0)) * pow(2, iteration);
    vec2 access10 = (base + vec2(1,0)) * pow(2, iteration);
    vec2 access01 = (base + vec2(0,1)) * pow(2, iteration);
    vec2 access11 = (base + vec2(1,1)) * pow(2, iteration);

    vec4 values00 = imageLoad(inputImage, ivec2(access00));
    vec4 values10 = imageLoad(inputImage, ivec2(access10));
    vec4 values01 = imageLoad(inputImage, ivec2(access01));
    vec4 values11 = imageLoad(inputImage, ivec2(access11));

    float avgLog = (values00.x + values01.x + values10.x + values11.x) / 4;
    float min = min(min(values00.y, values01.y), min(values10.y, values11.y));
    float max = max(max(values00.z, values01.z), max(values10.z, values11.z));

    imageStore(inputImage, ivec2(access00), vec4(avgLog, min, max, values00.w));
}
