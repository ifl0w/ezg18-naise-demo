#pragma once

#include "ComputeShader.hpp"

#include <glm/glm.hpp>
#include <memory>

namespace NAISE {
namespace Engine {

struct GPUParticle {
  glm::vec4 position; // position and ttl left
  glm::vec4 velocity; // velocity and initial ttl
};

class GPUParticleData {
public:
	GPUParticleData(std::string computeShaderPath, uint32_t maxParticles, float spawnRate);
	~GPUParticleData();

	std::shared_ptr<ComputeShader> computeShader;
	double spawnRate; // per second

	const uint32_t maxParticles;
	double accumSpawnCount = 0;

	uint8_t currentIdx = 0;

	gl::GLuint atomicCounter;
	gl::GLuint atomicSpawnCounter;
	gl::GLuint atomicCounterTempReadBuffer;

	gl::GLuint ssboParticles[2];

	gl::GLint lastCountLocation;
	gl::GLint maxCountLocation;
	gl::GLint deltaTimeLocation;
	gl::GLint spawnCountLocation;
	gl::GLint originTransformationLocation;
	gl::GLint invocationCountLocation;
};

}
}
