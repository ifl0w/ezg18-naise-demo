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

  void addUniforms(vector<std::string> requiredUniformLocations) {
	  for(const auto& uniformName: requiredUniformLocations) {
		  uniformLocations.insert(
				  std::pair(uniformName, uniformLocation(computeShader->shaderID, uniformName)));
	  }
  }

  std::unordered_map<std::string, gl::GLuint> uniformLocations;
};

class GPUParticleComponent: public Component {
public:
	std::unique_ptr<GPUParticleData> particleSystemData;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;
};

}
}

