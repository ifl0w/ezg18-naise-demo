#version 430

/*
 ----------- DO NOT CHANGE START -----------
 This block should not be changed and is required by the engine.

 Implement initParticle and updateParticle;
*/

struct GPUParticle {
  vec4 position; // position and ttl left
  vec4 velocity; // velocity and initial ttl
};

/*
 * Initialize and return a particle.
 */
GPUParticle initParticle(vec3 seed); // Implement this function!

/*
 * Update the received particle and return it.
 */
GPUParticle updateParticle(GPUParticle particle); // Implement this function!
/*
 * Called after the particle was inserted to the output list.
 * Can be used to change the transformation matrix of the particle.
 */
mat4 generateTransformationMatrix(GPUParticle particle); // Implement this function!

// choose a sufficient work group size:
layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout (binding = 0, offset = 0) uniform atomic_uint particleCounter; // current particle count (atomic counter)
layout (binding = 1, offset = 0) uniform atomic_uint spawnCounter; // needed for consisten spawning (synchronisation)

uniform uint lastCount;
uniform uint maxCount;
uniform uint spawnCount;
uniform float deltaTime;
uniform mat4 originTransformation;

layout(std430, binding = 2) buffer ParticlesIn
{
    GPUParticle particlesIn[];
};

layout(std430, binding = 3) buffer ParticlesOut
{
    GPUParticle particlesOut[];
};

layout(std430, binding = 4) buffer particleTransformations
{
    mat4 transformations[];
};

void addParticleToOutput(GPUParticle particle)
{
    if(particle.position.w <= 0) { // particle‘s still alive
        return;
    }

    // the next steps synchonize and the workgroups (ensure correct max number of particles)
    // increment the atomic counter and use its previous value:
    uint nr = atomicCounterIncrement(particleCounter);

    if (nr >= maxCount) {
        // we are out of memory
        atomicCounterDecrement(particleCounter);
        return;
    }

    particlesOut[nr].position = particle.position;
    particlesOut[nr].velocity = particle.velocity;
    transformations[nr] = originTransformation * generateTransformationMatrix(particle);
}

float random(vec3 co) {
	return fract(sin(dot(co, vec3(12.9898,78.233,45.164))) * 43758.5453);
}

void spawnParticle(vec3 seed) {
    uint nr = atomicCounterIncrement(spawnCounter);

    if (nr >= spawnCount) {
        atomicCounterDecrement(spawnCounter);
        return;
    }

    addParticleToOutput(initParticle(seed));
}

void main() {
    // unique one-dimensional index:
    uint idx = gl_GlobalInvocationID.x + gl_GlobalInvocationID.y * gl_NumWorkGroups.x * gl_WorkGroupSize.x;

    if(idx < lastCount) {
        addParticleToOutput(updateParticle(particlesIn[idx]));
    }

    while (atomicCounter(spawnCounter) < spawnCount) {
        spawnParticle(vec3(gl_GlobalInvocationID));
    }
}

/*
 ----------- DO NOT CHANGE END -----------
*/

#define M_PI 3.1415926535897932384626433832795

uniform float uRadius;
uniform float uEmitterHeight;
uniform int uNumberStrains;
uniform vec2 uLifeTime; // min, duration
uniform vec3 uVelocity; // collapse factor, suction, rotational velocity

mat4 generateTransformationMatrix(GPUParticle particle) {
    vec3 pos = particle.position.xyz;
    vec3 vel = particle.velocity.xyz;
    float initialTTL = particle.velocity.w;
    float TTL = particle.position.w;

    float ttlFactor = (initialTTL - TTL) / initialTTL;

    mat4 modelTransformation = mat4(1);
    modelTransformation[3][0] = pos.x;
    modelTransformation[3][1] = pos.y;
    modelTransformation[3][2] = pos.z;

    modelTransformation[0][0] = 1 - ttlFactor;
    modelTransformation[1][1] = 1 - ttlFactor;
    modelTransformation[2][2] = 1 - ttlFactor;

    return modelTransformation;
}

GPUParticle initParticle(vec3 seed) {
    GPUParticle result;

    float rand = random(seed / deltaTime);
    int bins = uNumberStrains;
    int i = int(bins*rand)*2;

    float t = random(seed * deltaTime) * M_PI/bins + ((M_PI)/bins)*(i);
    float x = uRadius * cos(t);
    float z = uRadius * sin(t);
    float y = -uEmitterHeight/2 + random(seed * deltaTime) * uEmitterHeight;

    vec3 spawnPos = vec3(x, y, z);
    float ttl = uLifeTime.x + random(spawnPos) * uLifeTime.y;
    vec3 spawnVelocity = vec3(pow(uVelocity.x, ttl), uVelocity.y, uVelocity.z); // scaling/collapsing factor, y velocity, angular velocity

    result.position = vec4(spawnPos, ttl);
    result.velocity = vec4(spawnVelocity, ttl);

    return result;
}

GPUParticle updateParticle(GPUParticle particle) {
    GPUParticle result;

    // update pos, speed, ...
    float ttlFactor = (particle.velocity.w - particle.position.w) / particle.velocity.w;

    vec3 velocity = particle.velocity.xyz;
    float angularVelocity = (velocity.z * deltaTime) * ttlFactor;

    float x = particle.position.x * cos(angularVelocity) - particle.position.z * sin(angularVelocity);
    float z = particle.position.z * cos(angularVelocity) + particle.position.x * sin(angularVelocity);
    float y = particle.position.y;

    vec3 pos = vec3(x, y, z) - (vec3(x, y, z) * particle.velocity.x * deltaTime); // scale down the vortex
    pos.y += particle.velocity.y * deltaTime;

    float ttl = particle.position.w - deltaTime;

    result.position = vec4(pos, ttl);
    result.velocity = particle.velocity;

    return result;
}
