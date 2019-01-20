#version 430

layout (location = 0) out vec4 blur;

layout(std140, binding = 0) uniform screenData
{
	int viewportWidth;
	int viewportHeight;
	int multiSampling;
	float brightnessFactor;
};

uniform sampler2D image;
uniform sampler2D reflection; //.a -> alpha value for blending


in vec2 TexCoords;


void main()
{
    vec4 result = vec4(0);
    vec4 original = texture(image, TexCoords);
    vec4 refl = texture(reflection, TexCoords);

    float alpha = refl.a;

    if(alpha > 0){
        // blending
        result = refl * (alpha) + original * (1-alpha);
    } else {
        result = original;
    }

	blur = result;
}

