#include <systems/particle-system/GPUParticleData.hpp>

#include <Resources.hpp>

using namespace NAISE::Engine;

GPUParticleData::GPUParticleData(std::string computeShaderPath, uint32_t maxParticles, float spawnRate)
		: maxParticles(maxParticles),
		  spawnRate(spawnRate) {
	computeShader = NAISE::Engine::Resources::getComputeShader(computeShaderPath);

	this->lastCountLocation = uniformLocation(computeShader->shaderID, "lastCount");
	this->maxCountLocation = uniformLocation(computeShader->shaderID, "maxCount");
	this->spawnCountLocation = uniformLocation(computeShader->shaderID, "spawnCount");
	this->deltaTimeLocation = uniformLocation(computeShader->shaderID, "deltaTime");
	this->originTransformationLocation = uniformLocation(computeShader->shaderID, "originTransformation");
	this->invocationCountLocation = uniformLocation(computeShader->shaderID, "invocationCount");

	glGenBuffers(1, &atomicCounter);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicCounter);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);
	GLuint initValue = 0;
	glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &initValue);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

	// Because of a performance warning when reading the atomic counter, we
	// create a buffer to move-to and read-from instead.
	glGenBuffers(1, &atomicCounterTempReadBuffer);
	glBindBuffer(GL_COPY_WRITE_BUFFER, atomicCounterTempReadBuffer);
	glBufferData(GL_COPY_WRITE_BUFFER, sizeof(GLuint), nullptr,
				 GL_DYNAMIC_READ);
	glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

	glGenBuffers(1, &atomicSpawnCounter);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicSpawnCounter);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &initValue);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

	// generating position and velocity buffers
	glGenBuffers(2, &ssboParticles[0]);
	for (int i = 0; i < 2; ++i) {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboParticles[i]);
		glBufferData(GL_SHADER_STORAGE_BUFFER, maxParticles * sizeof(GPUParticle), nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboParticles[i]);
		GLuint zero = 0;
		glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED, GL_UNSIGNED_INT, &zero);
	}
}

GPUParticleData::~GPUParticleData() {
	glDeleteBuffers(1, &atomicCounter);
	glDeleteBuffers(1, &atomicCounterTempReadBuffer);
	glDeleteBuffers(1, &atomicSpawnCounter);
	glDeleteBuffers(2, &ssboParticles[0]);
}
