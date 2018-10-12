#version 430
#define PI 3.1415926535897932384626433832795

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

layout(std140, binding = 1) uniform projectionData
{
    mat4 viewProjection;    // size = 64B
    mat4 projectionMatrix;    // size = 64B
    mat4 viewMatrix;    // size = 64B
    vec3 cameraPosition;    // size = 16B
};

uniform bool useInstancing = false;

layout(std430, binding = 4) buffer particleTransformations
{
    mat4 instanceTransformations[];
};

uniform vec3 color;
uniform mat4 modelMatrix;

/**WATERMESH ANIMATION**/
uniform float wavelength;
uniform float waveAmplitude;
uniform float time;
uniform bool useWatermeshAnimation;
uniform int type;

vec3 newPosition;
vec3 newNormal;
float l; //wavelength

out vec3 vNorm;


/** Watermesh Animation Variants**/

// Type: 1
void sinusWave_DistortionAlongX(){
    newPosition = newPosition + (waveAmplitude *2*sin(position.x/l + time))*normal;
}

// Type: 2
void sinusWave_DistortionAlongZ(){
    newPosition = newPosition + (waveAmplitude *2*sin(position.z/l + time))*normal;
}

// Type: 3
void sinuscosinusWave_HeightDistortionAlongXZ(){
    newPosition = newPosition + ((waveAmplitude * (sin(position.z/l + time) + cos(position.x/l + time))))*normal;
}

// Type: 4
void sinuscosinusWave_HeightDistortionAlongXZY(){
    float x = mod(position.x + position.z * position.x * 0.5, l)/l + time * mod(position.x * 0.8 + position.z, 1.5);
    float z = mod(0.5 * (position.z * position.x + position.x * position.z), l)/l + time * 2 * mod(position.x, 2);
    newPosition = newPosition + (waveAmplitude * (sin(z) + cos(x)));
}

// Type: 5
void crazy(){
    float x = position.x;
    float z = position.z;
    float xPos = (x+z *x)/l + time;
    float zPos = (z+x *z)/l + time * 2;
    newPosition = newPosition + (waveAmplitude * ( sin(zPos) + cos(zPos) + cos(xPos)));
}

void main() {

    newPosition = position;
    l = wavelength;
    if(l == 0){
        l = 0.0000000000000000000000001; // prevent div 0
    }
    if(useWatermeshAnimation){
        switch(type){
            case 1:
            sinusWave_DistortionAlongX();
            break;
            case 2:
            sinusWave_DistortionAlongZ();
            break;
            case 3:
            sinuscosinusWave_HeightDistortionAlongXZ();
            break;
            case 4:
            sinuscosinusWave_HeightDistortionAlongXZY();
            break;
            case 5:
            crazy();
            break;
            default:
            sinusWave_DistortionAlongX();
            break;
        }
    }
    vNorm = normal;

    mat4 mMatrix = modelMatrix;
    if(useInstancing) {
        mMatrix = instanceTransformations[gl_InstanceID];
    }

    gl_Position = viewProjection * mMatrix * vec4(newPosition, 1);
}