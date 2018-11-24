#pragma once

#include <systems/System.hpp>

#include "GPUParticleData.hpp"
#include "MeshParticleComponent.hpp"

namespace NAISE {
namespace Engine {

class GPUMeshParticleSystem {
protected:
	void activate(GPUParticleData& particleData, MeshParticleComponent& meshParticleComponent,
				  glm::mat4 originTransform, float deltaTime);

	void execute(GPUParticleData& particleData, MeshParticleComponent& meshParticleComponent);
};

}
}
