#version 430

layout(binding=0, rgba16f) uniform readonly image2D inputImage;
layout(binding=1, rgba16f) uniform image2D outputImage;

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

void main() {
    vec4 color = imageLoad(inputImage, ivec2(gl_GlobalInvocationID.xy));
    vec3 factors = vec3(0.2126, 0.7152, 0.0722);

    float lum =  max(0, log(factors.r * color.r + factors.g * color.g + factors.b * color.b));

    vec4 prevLum = imageLoad(outputImage, ivec2(gl_GlobalInvocationID.xy));
    imageStore(outputImage, ivec2(gl_GlobalInvocationID.xy), vec4(lum, lum, lum, prevLum.w));
}
