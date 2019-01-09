#version 430

layout (location = 0) out vec4 reflections;
in vec2 TexCoords;


layout(std140, binding = 0) uniform screenData
{
	int viewportWidth;
	int viewportHeight;
	int multiSampling;
	float brightnessFactor;
};

uniform sampler2D gPosition;
uniform sampler2D imageInput;

void main()
{
    vec2 resolution = vec2(viewportWidth, viewportHeight);
    vec2 normalizedTexCoords = vec2(gl_FragCoord.xy / resolution);

    // TODO: change normalizedTexCoords with TexCoords
    vec4 depth = texture(gPosition, normalizedTexCoords);
    vec4 image = texture(imageInput, normalizedTexCoords);
   reflections = vec4(vec3(depth.a), 1);
   reflections = vec4(image);
  //reflections = vec4(1,0,0, 1);
}
