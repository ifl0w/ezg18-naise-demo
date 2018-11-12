#include "WeldingParticleSystem.hpp"

void WeldingParticleSystem::setUniforms(NAISE::Engine::Entity& particleSystem) {
	auto& data = particleSystem.component<GPUParticleComponent>().particleSystemData;

	glUniform1f(uniformLocation(data->computeShader->shaderID, "uRadius"), 10.0f);
	glUniform3fv(uniformLocation(data->computeShader->shaderID, "uVelocity"), 1, value_ptr(vec3(0,1.0f,0)));
	glUniform2fv(uniformLocation(data->computeShader->shaderID, "uLifeTime"), 1, value_ptr(vec2(4.0f, 7.0f)));
}
