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

struct GPUParticle {
  glm::vec4 position; // position and ttl left
  glm::vec4 velocity; // velocity and initial ttl
};

struct GPUParticleData {
  GPUParticleData(std::string computeShaderPath, uint32_t maxParticles, float spawnRate);
  ~GPUParticleData();

  std::shared_ptr<ComputeShader> computeShader;
  double spawnRate; // per second

  const uint32_t maxParticles;
  double accumSpawnCount = 0;

  uint8_t currentIdx = 0;
  uint32_t particleCount;

  gl::GLuint atomicCounter;
  gl::GLuint atomicSpawnCounter;
  gl::GLuint atomicCounterTempReadBuffer;

  gl::GLuint ssboParticles[2];
  gl::GLuint ssboTransformations;

  gl::GLint lastCountLocation;
  gl::GLint maxCountLocation;
  gl::GLint deltaTimeLocation;
  gl::GLint spawnCountLocation;
  gl::GLint originTransformationLocation;
};

class GPUParticleComponent: public Component {
public:
	explicit GPUParticleComponent(uint32_t maxParticles): maxParticles(maxParticles) {
		// generate tranformation buffer
		glGenBuffers(1, &ssboTransformations);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboTransformations);
		glBufferData(GL_SHADER_STORAGE_BUFFER, maxParticles * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	~GPUParticleComponent() {
		glDeleteBuffers(1, &ssboTransformations);
	}

	std::unique_ptr<GPUParticleData> particleSystemData;

	const uint32_t maxParticles;
	uint32_t particleCount{};
	gl::GLuint ssboTransformations{};

	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;
};

}
}

