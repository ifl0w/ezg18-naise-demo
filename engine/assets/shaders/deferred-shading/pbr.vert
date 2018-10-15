#version 430

#define PI 3.1415926535897932384626433832795

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 tangent;

uniform bool useInstancing = false;
layout(std430, binding = 4) buffer particleTransformations
{
    mat4 instanceTransformations[];
};

layout(std140, binding = 1) uniform projectionData
{
    mat4 viewProjection;    // size = 64B
    mat4 projectionMatrix;    // size = 64B
    mat4 viewMatrix;    // size = 64B
    vec3 cameraPosition;    // size = 16B
};

uniform mat4 modelMatrix;

uniform bool useNormalTexture;

out vec3 vPos;
out vec3 vNorm;
out vec2 vUV;
out mat3 TBN;
out vec3 Reflection;

/**WATERMESH ANIMATION**/
uniform float wavelength;
uniform float waveAmplitude;
uniform float time;
uniform bool useWatermeshAnimation;
uniform int type;

vec3 newPosition;
vec3 newNormal;
float l; //wavelength

/** Watermesh Animation Variants**/

// Type: 1
void sinusWave_DistortionAlongX(){
    newPosition = newPosition + (waveAmplitude * 2 * sin(position.x / l + time)) * normal;
    newNormal = normalize(vec3(((waveAmplitude * 2 * cos(newPosition.x / l + time)) / l) * -1,1,0));
}

// Type: 2
void sinusWave_DistortionAlongZ(){
    newPosition = newPosition + (waveAmplitude * 2 * sin(position.z / l + time)) * normal;
    newNormal = normalize(vec3(0, 1, ((waveAmplitude * 2 * cos(position.z / l + time)) / l) * -1));
}

// Type: 3
void sinuscosinusWave_HeightDistortionAlongXZ(){
    newPosition = newPosition + ((waveAmplitude * (sin(position.z / l + time) + cos(position.x / l + time)))) * normal;
    float xGrad = -1 * (waveAmplitude * ((sin(newPosition.x / l + time)) / l));
    float zGrad = (waveAmplitude * ((cos(newPosition.z / l + time)) / l));
    newNormal = normalize(vec3(-1 * xGrad, 1, -1 * zGrad));
}

// Type: 4
void sinuscosinusWave_HeightDistortionAlongXZY(){
    float x = mod(position.x + position.z * position.x * 0.5, l) / l + time * mod(position.x * 0.8 + position.z, 1.5);
    float z = mod(0.5 * (position.z * position.x + position.x * position.z), l) / l + time * 2 * mod(position.x, 2);
    newPosition = newPosition + (waveAmplitude * (sin(z) + cos(x)));
    float xGrad = -1 * waveAmplitude * ((sin(newPosition.x / l + time)) / 1);
    float zGrad = waveAmplitude * ((cos(newPosition.z / l + time)) / 1);
    newNormal = normalize(vec3(-1 * xGrad, 1, -1 * zGrad));
}

// Type: 5
void crazy(){
    float x = position.x;
    float z = position.z;
    float xPos = (x + z * x) / l + time;
    float zPos = (z + x * z) / l + time * 2;

    newPosition = newPosition + (waveAmplitude * ( sin(zPos) + cos(zPos) + cos(xPos)));

    float w = (z * x + x) / l;
    float firstPart = ((z + 1) * sin(w + time) + z * sin(w + 2 * time) - z * cos((z * x + z) / l + 2 * time));
    float xGrad = -1 * ((waveAmplitude *  firstPart) / l);
    w = (z * x + z) / l;
    firstPart = ((x + 1) * sin(w + 2 * time) + (-x - 1) * cos(w + 2 * time) + x * sin((z * x + x) / l + time));
    float zGrad = -1 * ((waveAmplitude *  firstPart) / l);
    /*
    Wrong Normals with cool effect
    float tmp = (cos(z*x+x *l+time)+sin(z*x+z*l+2*time)+cos(z*x+z*l+2*time));
    float xGrad = waveAmplitude *(tmp);
    tmp = (sin(x*z+z*l+2*time)+cos(x*z+z*l+2*time)+cos(x*z+x*l+time));
    float zGrad = waveAmplitude *(tmp);*/
    newNormal = normalize(vec3(-1 * xGrad, 1, -1 * zGrad));
}


void main() {
    mat4 mMatrix = modelMatrix;

    if(useInstancing) {
        mMatrix = instanceTransformations[gl_InstanceID];
    }

    TBN = mat3(0);
    if (useNormalTexture) {
        // tangent space
        vec3 T = normalize(vec3(mMatrix * vec4(tangent, 0.0)));
        vec3 N = normalize(vec3(mMatrix * vec4(normal, 0.0)));
        vec3 B = cross(N, T);
        TBN = mat3(T, B, N);
    }

    newPosition = position;
    newNormal = normal;
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

    vec4 pos = mMatrix * vec4(newPosition, 1);
	vPos = vec3(pos);

	vUV = uv;

    mat4 normalMatrix = transpose(inverse(mMatrix));
    vNorm = normalize(vec3(normalMatrix * vec4(newNormal, 1)));

     vec3 ViewDirection = (vPos - cameraPosition);
     Reflection  = reflect(ViewDirection, vNorm);

	gl_Position = viewProjection * pos;

}