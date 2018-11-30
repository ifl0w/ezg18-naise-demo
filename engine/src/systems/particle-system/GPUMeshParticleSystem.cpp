#include <systems/particle-system/GPUMeshParticleSystem.hpp>

void GPUMeshParticleSystem::activate(GPUParticleData& particleData, MeshParticleComponent& meshParticleComponent,
								 glm::mat4 originTransform, float deltaTime) {
	particleData.computeShader->useShader();

	glUniformMatrix4fv(particleData.originTransformationLocation, 1, false, glm::value_ptr(originTransform));
	glUniform1ui(particleData.lastCountLocation, meshParticleComponent.particleCount);
	glUniform1ui(particleData.maxCountLocation, meshParticleComponent.maxParticles);

	particleData.accumSpawnCount += particleData.spawnRate * deltaTime;
	GLuint spawnCount = 0;
	if (particleData.accumSpawnCount >= 1) {
		spawnCount += particleData.accumSpawnCount;
		particleData.accumSpawnCount -= spawnCount;
	}
	glUniform1ui(particleData.spawnCountLocation, spawnCount);
	glUniform1f(particleData.deltaTimeLocation, deltaTime);

	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, particleData.atomicSpawnCounter);
	GLuint tmp = 0;
	glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &tmp);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
}

void GPUMeshParticleSystem::execute(GPUParticleData& particleData, MeshParticleComponent& meshParticleComponent) {
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, particleData.atomicCounter);
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 1, particleData.atomicSpawnCounter);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, particleData.ssboParticles[particleData.currentIdx]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, particleData.ssboParticles[!particleData.currentIdx]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, meshParticleComponent.ssboTransformations);

	auto groups = static_cast<GLuint>((meshParticleComponent.particleCount / (16 * 16)) + 1);
	particleData.computeShader->compute(groups);

	particleData.currentIdx = static_cast<uint8_t>(!particleData.currentIdx); // ping-pong between buffers

	glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);
	// Read atomic counter
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, particleData.atomicCounter);
	glBindBuffer(GL_COPY_WRITE_BUFFER, particleData.atomicCounterTempReadBuffer);
	// from atomic counter to temp buffer:
	glCopyBufferSubData(GL_ATOMIC_COUNTER_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, sizeof(GLuint));

	auto counterValue = (GLuint*) glMapBufferRange(GL_COPY_WRITE_BUFFER, 0, sizeof(GLuint),
												   GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
	meshParticleComponent.particleCount = counterValue[0];
	counterValue[0] = 0; // reset atomic counter
	glUnmapBuffer(GL_COPY_WRITE_BUFFER); // stop writing to buffer
	// copy temp buffer to atomic counter:
	glCopyBufferSubData(GL_COPY_WRITE_BUFFER, GL_ATOMIC_COUNTER_BUFFER, 0, 0, sizeof(GLuint));
	// Make sure everything from Compute-Shader is written
	glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);
}

