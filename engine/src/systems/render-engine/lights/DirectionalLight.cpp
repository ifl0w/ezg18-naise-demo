#include <glm/gtc/matrix_transform.hpp>
#include <systems/render-engine/lights/DirectionalLight.hpp>
#include <systems/render-engine/materials/shaders/Shader.hpp>
#include <systems/render-engine/shadow-map/CascadedShadowMapper.hpp>
//#include "../cameras/Camera.h"
//#include "../materials/shaders/Shader.h"

using namespace NAISE::Engine;
using namespace NAISE::RenderCore;
using namespace glm;

DirectionalLight::DirectionalLight()
		: DirectionalLight(vec3(1, 1, 1), vec3(0, 1, 0)) {
}

DirectionalLight::DirectionalLight(vec3 color, vec3 direction)
		: Light(vec3(0), color, color, color * 0.2f, 1, 0, 0, 360, 0, direction, true) {
}

mat4 DirectionalLight::getShadowMatrix()const {
	return glm::lookAt(vec3(0), vec3(data.direction), vec3(0,1,0));
}

mat4 DirectionalLight::getProjectionMatrix(AABB aabb) const {
	aabb.transform(getShadowMatrix());

	float minX = aabb.values.first.x;
	float minY = aabb.values.first.y;
	float minZ = aabb.values.first.z;
	float maxX = aabb.values.second.x;
	float maxY = aabb.values.second.y;
	float maxZ = aabb.values.second.z;

	return glm::ortho(minX, maxX, minY, maxY, -maxZ - maxShadowDistance, -minZ); // 5000 is the max shadow distance
}

void DirectionalLight::addShadowMapper() {
	std::vector cascades = {
			Cascade{vec2(1024,1024), vec2(0, 5)},
			Cascade{vec2(512,512), vec2(5, 50)},
			Cascade{vec2(256,256), vec2(50, 100)},
	};

	this->shadowMapper = std::make_unique<CascadedShadowMapper>(cascades);
}

//void DirectionalLight::update(std::chrono::microseconds deltaTime) {
//	Light::update(deltaTime);
//
//	auto aabb = AABB(Shader::activeCamera->frustum.getBoundingVolume(50));
//	frustum = Frustum(aabb, getShadowMatrix(), maxShadowDistance);
//}
