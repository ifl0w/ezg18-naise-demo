#pragma once

#include "ComputeShader.hpp"
#include "MeshParticleComponent.hpp"
#include "SimpleGPUParticleComponent.hpp"
#include "GPUMeshParticleSystem.hpp"

#include <glm/glm.hpp>
#include <glbinding/gl/gl.h>
#include <chrono>
#include <memory>
#include <systems/System.hpp>

#include <components/TransformComponent.hpp>
#include <components/AABBComponent.hpp>

namespace NAISE {
namespace Engine {

struct SimpleGPUParticleDataSignature: public Signature<MeshParticleComponent, SimpleGPUParticleComponent> {};

class SimpleGPUParticleSystem: public GPUMeshParticleSystem, public System {
public:
	SimpleGPUParticleSystem();

	void process(microseconds deltaTime) override;
};

}
}
