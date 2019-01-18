#version 430

layout(binding=0, rgba16f) uniform readonly image2D inputImage;
layout(binding=1, rgba16f) uniform readonly image2D luminance;
layout(binding=2, rgba16f) uniform image2D combined;

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

void main() {
    vec4 color = imageLoad(inputImage, ivec2(gl_GlobalInvocationID.xy));
    vec4 lum = imageLoad(luminance, ivec2(gl_GlobalInvocationID.xy));
    vec3 factors = vec3(0.2126, 0.7152, 0.0722);
    float currLum =  max(0, log(factors.r * color.r + factors.g * color.g + factors.b * color.b));

    vec4 current = imageLoad(combined, ivec2(gl_GlobalInvocationID.xy));

    float threshold = log(lum.w)*1.5;
    float value = lum.x;
    if (threshold < value) {
        imageStore(combined, ivec2(gl_GlobalInvocationID.xy), color * (value - threshold)/value);
    }
}
