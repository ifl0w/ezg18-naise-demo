#include <systems/particle-system/SimpleGPUParticleSystem.hpp>
#include <Resources.hpp>
#include <utility>

#include <Engine.hpp>

using namespace NAISE::Engine;

SimpleGPUParticleSystem::SimpleGPUParticleSystem() {
	Engine::getEntityManager().addSignature<SimpleGPUParticleDataSignature>();
}

void SimpleGPUParticleSystem::process(microseconds deltaTime) {
	auto& particleSystems = Engine::getEntityManager().getEntities<SimpleGPUParticleDataSignature>();
	for (auto particleSystem: particleSystems) {
		auto& simpleGPUParticleComponent = particleSystem->component<SimpleGPUParticleComponent>();
		auto& particleData = *simpleGPUParticleComponent.particleSystemData;
		auto& meshParticleComponent = particleSystem->component<MeshParticleComponent>();
		auto& transformComponent = particleSystem->component<TransformComponent>();

		std::chrono::duration<float> deltaTimeSec = deltaTime;
		activate(particleData, meshParticleComponent, transformComponent.getModelMatrix() ,deltaTimeSec.count());

		if (simpleGPUParticleComponent.setUniforms) {
			simpleGPUParticleComponent.setUniforms(particleData.computeShader->shaderID);
		}
		
		execute(particleData, meshParticleComponent);
	}
}
