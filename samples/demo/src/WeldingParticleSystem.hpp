#pragma once

#include <systems/particle-system/GPUParticleSystem.hpp>
#include <components/Component.hpp>

struct WeldingParticleComponent: public NAISE::Engine::Component {

};

class WeldingParticleSystem: public NAISE::Engine::GPUParticleSystem {
	void setUniforms(NAISE::Engine::Entity& particleSystem) override;
};


