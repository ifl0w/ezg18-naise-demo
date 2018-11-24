#pragma once

#include "GPUParticleData.hpp"

#include <components/Component.hpp>
#include <glbinding/gl/gl.h>
#include <glm/glm.hpp>
#include <memory>

#include <systems/render-engine/meshes/Mesh.hpp>
#include <systems/render-engine/materials/Material.hpp>

namespace NAISE {
namespace Engine {

class SimpleGPUParticleComponent : public Component {
public:
	SimpleGPUParticleComponent()
			: SimpleGPUParticleComponent(5000, 500) {}

	SimpleGPUParticleComponent(uint32_t maxParticles, float spawnRate)
			: SimpleGPUParticleComponent("resources/particle-systems/simple-particles.glsl", maxParticles, spawnRate) {}

	SimpleGPUParticleComponent(std::string computeShaderPath, uint32_t maxParticles, float spawnRate) {
		particleSystemData = make_unique<GPUParticleData>(computeShaderPath, maxParticles, spawnRate);
	}

	std::unique_ptr<GPUParticleData> particleSystemData;

	std::function<void (gl::GLuint shaderProgram)> setUniforms;
};

}
}

