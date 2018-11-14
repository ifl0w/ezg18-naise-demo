#pragma once

#include <systems/particle-system/GPUParticleSystem.hpp>
#include <components/Component.hpp>

class RaisingParticleSystem: public NAISE::Engine::GPUParticleSystem {
	void setUniforms(NAISE::Engine::Entity& particleSystem) override;
};


