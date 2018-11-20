#pragma once

#include "ComputeShader.hpp"
#include "GPUParticleComponent.hpp"

#include <glm/glm.hpp>
#include <glbinding/gl/gl.h>
#include <chrono>
#include <memory>
#include <systems/System.hpp>

#include <components/TransformComponent.hpp>
#include <components/AABBComponent.hpp>

namespace NAISE {
namespace Engine {

struct GPUParticleDataSignature: public Signature<GPUParticleComponent> {};

class GPUParticleSystem: public System {
public:
	GPUParticleSystem();

	void process(microseconds deltaTime) override;
};

}
}
