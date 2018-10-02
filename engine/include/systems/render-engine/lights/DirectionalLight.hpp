#pragma once

#include "Light.hpp"
#include "../frustum-culling/AABB.h"
#include "../frustum-culling/Frustum.h"

namespace NAISE {
namespace Engine {

class DirectionalLight :
		public LightComponent {
public:
	DirectionalLight();

	DirectionalLight(vec3 color, vec3 direction);

	void update(std::chrono::microseconds deltaTime) override;

	mat4 getShadowMatrix() const override;
	mat4 getProjectionMatrix() const override;

	Frustum frustum;
	float maxShadowDistance = 5000;
};

}
}