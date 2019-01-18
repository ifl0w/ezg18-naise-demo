#version 430

layout(location = 1) out vec4 FragColor;
layout(location = 2) out vec4 gPosition;

in vec3 TexCoords;

uniform vec3 backgroundColor;
uniform samplerCube skyboxTexture;
uniform bool useSkyboxTexture;
uniform float brightness = 1000;


void main()
{
    gl_FragDepth = 1.0;

    vec4 color = vec4(backgroundColor, 1);
    if(useSkyboxTexture) {
        color *= texture(skyboxTexture, TexCoords);
    }

    gPosition = vec4(TexCoords * 1000000, 0);
    FragColor = color * brightness;
}