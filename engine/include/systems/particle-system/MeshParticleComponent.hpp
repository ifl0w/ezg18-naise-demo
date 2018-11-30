#pragma once

#include <components/Component.hpp>
#include <glbinding/gl/gl.h>
#include <glm/glm.hpp>
#include <memory>
#include "ComputeShader.hpp"

#include <systems/render-engine/meshes/Mesh.hpp>
#include <systems/render-engine/materials/Material.hpp>

namespace NAISE {
namespace Engine {

class MeshParticleComponent: public Component {
public:
	explicit MeshParticleComponent(uint32_t maxParticles): maxParticles(maxParticles) {
		// generate tranformation buffer
		glGenBuffers(1, &ssboTransformations);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboTransformations);
		glBufferData(GL_SHADER_STORAGE_BUFFER, maxParticles * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	~MeshParticleComponent() {
		glDeleteBuffers(1, &ssboTransformations);
	}

	const uint32_t maxParticles;
	uint32_t particleCount{};
	gl::GLuint ssboTransformations{};

	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;
};

}
}

