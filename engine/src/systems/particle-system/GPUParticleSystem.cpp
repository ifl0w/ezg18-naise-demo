#include <systems/particle-system/GPUParticleSystem.hpp>
#include <Resources.hpp>
#include <utility>

#include <Engine.hpp>

using namespace NAISE::Engine;

GPUParticleSystem::GPUParticleSystem() {
	Engine::getEntityManager().addSignature<GPUParticleSignature>();
}

void GPUParticleSystem::process(microseconds deltaTime) {

	auto& particleSystems = Engine::getEntityManager().getEntities<GPUParticleSignature>();
	for (auto particleSystem: particleSystems) {
		auto& particleData = *particleSystem->component<GPUParticleComponent>().particleSystemData.get();;
		auto& transformComponent = particleSystem->component<TransformComponent>();

		particleData.computeShader->useShader();

		this->setUniforms(*particleSystem);

		mat4 originTransformation = transformComponent.getModelMatrix();
		glUniformMatrix4fv(particleData.originTransformationLocation, 1, false, glm::value_ptr(originTransformation));
		glUniform1ui(particleData.lastCountLocation, particleData.particleCount);
		glUniform1ui(particleData.maxCountLocation, particleData.maxParticles);
		std::chrono::duration<float> deltaTimeSec = deltaTime;
		particleData.accumSpawnCount += particleData.spawnRate * deltaTimeSec.count();
		GLuint spawnCount = 0;
		if (particleData.accumSpawnCount >= 1) {
			spawnCount += particleData.accumSpawnCount;
			particleData.accumSpawnCount -= spawnCount;
		}
		glUniform1ui(particleData.spawnCountLocation, spawnCount);
		glUniform1f(particleData.deltaTimeLocation, deltaTimeSec.count());

		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, particleData.atomicSpawnCounter);
		GLuint tmp = 0;
		glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &tmp);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

		glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, particleData.atomicCounter);
		glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 1, particleData.atomicSpawnCounter);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, particleData.ssboParticles[particleData.currentIdx]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, particleData.ssboParticles[!particleData.currentIdx]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, particleData.ssboTransformations);

		auto groups = static_cast<GLuint>((particleData.particleCount / (16 * 16)) + 1);
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
		particleData.particleCount = counterValue[0];
		counterValue[0] = 0; // reset atomic counter
		glUnmapBuffer(GL_COPY_WRITE_BUFFER); // stop writing to buffer
		// copy temp buffer to atomic counter:
		glCopyBufferSubData(GL_COPY_WRITE_BUFFER, GL_ATOMIC_COUNTER_BUFFER, 0, 0, sizeof(GLuint));
		// Make sure everything from Compute-Shader is written
		glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);
	}
}
