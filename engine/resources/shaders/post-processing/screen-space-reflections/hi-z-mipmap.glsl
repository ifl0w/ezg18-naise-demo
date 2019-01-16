#version 430

layout (location = 0) out float gLinearDepth;

uniform sampler2D lastImage;
uniform bool first;

in vec2 TexCoords;

void main()
{
    float result;

    if(first){
        result = texture( lastImage, TexCoords ).r;
    } else {
        vec4 minDepth;
        minDepth.r = texture( lastImage, TexCoords ).r; // current depth
        minDepth.g = textureOffset( lastImage, TexCoords, ivec2(0, -1) ).r;
        minDepth.b = textureOffset( lastImage, TexCoords, ivec2(-1,0) ).r;
        minDepth.a = textureOffset( lastImage, TexCoords, ivec2( -1,-1) ).r;

        result = min( min(minDepth.r, minDepth.g), min(minDepth.b, minDepth.a));
    }

    gLinearDepth = result;
}