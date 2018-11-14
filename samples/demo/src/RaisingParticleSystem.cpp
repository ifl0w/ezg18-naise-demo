#include "RaisingParticleSystem.hpp"

void RaisingParticleSystem::setUniforms(NAISE::Engine::Entity& particleSystem) {
	auto& data = particleSystem.component<GPUParticleComponent>().particleSystemData;

	glUniform1f(data->uniformLocations["uRadius"], 10.0f);
	glUniform3fv(data->uniformLocations["uVelocity"], 1, value_ptr(vec3(0,1.0f,0)));
	glUniform2fv(data->uniformLocations["uLifeTime"], 1, value_ptr(vec2(4.0f, 7.0f)));
}
