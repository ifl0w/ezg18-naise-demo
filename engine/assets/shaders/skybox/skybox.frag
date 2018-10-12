#version 430

out vec4 FragColor;

in vec3 TexCoords;

uniform vec3 backgroundColor;
uniform samplerCube skyboxTexture;
uniform bool useSkyboxTexture;

void main()
{
    gl_FragDepth = 1.0;

    vec4 color = vec4(backgroundColor, 1);
    if(useSkyboxTexture) {
        color *= texture(skyboxTexture, TexCoords);
    }

    FragColor = color;
}