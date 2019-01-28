#version 430

layout (location = 0) out vec2 gLinearDepth;

uniform sampler2D lastImage;
//uniform ivec2 lastSize;
//uniform ivec2 offset;
uniform bool first;

in vec2 TexCoords;

void main()
{
    vec2 result;

    if(first){
        result.r = texture( lastImage, TexCoords ).r;
        result.g = texture( lastImage, TexCoords ).r;
    } else {
        vec4 minDepth;
        vec4 maxDepth;

        const int offset = 1;

        minDepth.r = texture( lastImage, TexCoords ).r; // current depth
        minDepth.g = textureOffset( lastImage, TexCoords, ivec2(0, offset) ).r;
        minDepth.b = textureOffset( lastImage, TexCoords, ivec2(offset,0) ).r;
        minDepth.a = textureOffset( lastImage, TexCoords, ivec2( offset,offset) ).r;

        maxDepth.r = texture( lastImage, TexCoords ).g; // current depth
        maxDepth.g = textureOffset( lastImage, TexCoords, ivec2(0, offset) ).g;
        maxDepth.b = textureOffset( lastImage, TexCoords, ivec2(offset,0) ).g;
        maxDepth.a = textureOffset( lastImage, TexCoords, ivec2(offset,offset) ).g;


        result.r = min( min(minDepth.r, minDepth.g), min(minDepth.b, minDepth.a));
        result.g = max( max(maxDepth.r, maxDepth.g), max(maxDepth.b, maxDepth.a));
    }

    gLinearDepth = result;
}