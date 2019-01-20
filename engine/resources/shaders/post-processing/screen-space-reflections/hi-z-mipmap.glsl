#version 430

layout (location = 0) out vec2 gLinearDepth;

uniform sampler2D lastImage;
uniform ivec2 lastSize;
uniform ivec2 offset;
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

        int offs = 1;

       minDepth.r = texture( lastImage, TexCoords ).r; // current depth
        minDepth.g = textureOffset( lastImage, TexCoords, ivec2(0, offs) ).r;
        minDepth.b = textureOffset( lastImage, TexCoords, ivec2(offs,0) ).r;
        minDepth.a = textureOffset( lastImage, TexCoords, ivec2( offs,offs) ).r;

        maxDepth.r = texture( lastImage, TexCoords ).g; // current depth
        maxDepth.g = textureOffset( lastImage, TexCoords, ivec2(0, offs) ).g;
        maxDepth.b = textureOffset( lastImage, TexCoords, ivec2(offs,0) ).g;
        maxDepth.a = textureOffset( lastImage, TexCoords, ivec2(offs,offs) ).g;

        /* Fetch texels from current MIP LOD */
        ivec2 coord = ivec2(gl_FragCoord.xy) * ivec2(2);
        ivec2 off = offset;
//coord = ivec2(gl_FragCoord.xy);
     /*   minDepth.r = texelFetch(lastImage, coord                            , 0).r;
        minDepth.g = texelFetch(lastImage, coord + ivec2(off.x,        0), 0).r;
        minDepth.b = texelFetch(lastImage, coord + ivec2(  0, off.y), 0).r;
        minDepth.a = texelFetch(lastImage, coord + ivec2(off.x, off.y), 0).r;

        maxDepth.r = texelFetch(lastImage, coord                            , 0).g;
        maxDepth.g = texelFetch(lastImage, coord + ivec2(off.x,        0), 0).g;
        maxDepth.b = texelFetch(lastImage, coord + ivec2(  0, off.y), 0).g;
        maxDepth.a = texelFetch(lastImage, coord + ivec2(off.x, off.y), 0).g;*/


      /*  minDepth.r = texture( lastImage, TexCoords ).r; // current depth
        minDepth.g = textureOffset( lastImage, TexCoords, ivec2(0, offset.y) ).r;
        minDepth.b = textureOffset( lastImage, TexCoords, ivec2(offset.x, 0) ).r;
        minDepth.a = textureOffset( lastImage, TexCoords, ivec2(offset.x, offset.y) ).r;

        maxDepth.r = texture( lastImage, TexCoords ).g; // current depth
        maxDepth.g = textureOffset( lastImage, TexCoords, ivec2(0, offset.y) ).g;
        maxDepth.b = textureOffset( lastImage, TexCoords, ivec2(offset.x, 0) ).g;
        maxDepth.a = textureOffset( lastImage, TexCoords, ivec2(offset.x, offset.y) ).g;*/

        result.r = min( min(minDepth.r, minDepth.g), min(minDepth.b, minDepth.a));
        result.g = max( max(maxDepth.r, maxDepth.g), max(maxDepth.b, maxDepth.a));
    }

    gLinearDepth = result;
}