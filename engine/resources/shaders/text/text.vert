#version 430

//layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 tangent;

uniform bool useInstancing = false;
layout(std430, binding = 0) buffer InstanceTransforms
{
    mat4 instanceTransforms[];
};

layout(std140, binding = 1) uniform projectionData
{
    mat4 viewProjection;    // size = 64B
    mat4 projectionMatrix;    // size = 64B
    mat4 viewMatrix;    // size = 64B
    vec3 cameraPosition;    // size = 16B
};

uniform mat4 modelMatrix;

void main()
{
    gl_Position = modelMatrix * vec4(position, 1.0);
    //TexCoords = vertex.zw;
}
