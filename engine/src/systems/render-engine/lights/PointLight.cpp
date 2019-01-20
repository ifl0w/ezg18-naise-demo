#include <systems/render-engine/lights/PointLight.hpp>
#include <systems/render-engine/lights/DirectionalLight.hpp>
#include <systems/render-engine/materials/shaders/Shader.hpp>

//#include "../cameras/Camera.h"
//#include "../Engine.h"
//#include "../materials/shaders/Shader.h"

using namespace NAISE::RenderCore;

PointLight::PointLight()
		: PointLight(vec3(0)) {
}

PointLight::PointLight(vec3 position)
		: PointLight(position, vec3(1)) {
}

PointLight::PointLight(vec3 position, vec3 color)
		: Light(position, color, color, color, 1.0, 0.7, 1.8, 360, 0, vec3(0), false) {

//	this->scale = vec3(calculateLightVolumeRadius());
}

void PointLight::setAttenuation(float constant, float linear, float quadratic) {
	data.attConstant = constant;
	data.attLinear = linear;
	data.attQuadratic = quadratic;

//	this->scale = vec3(calculateLightVolumeRadius());
}

float PointLight::calculateLightVolumeRadius(glm::vec3 ambientIntensity) {
	float lightMax = glm::max(glm::max(data.diffuse.r, data.diffuse.g), data.diffuse.b);
	float minLightValue = (ambientIntensity.x + ambientIntensity.y + ambientIntensity.z) * 0.01f; // minimum 0.1% of ambient light;

	double radius = sqrt(glm::pow(minLightValue, -1) * lightMax);

	return static_cast<float>(radius); // light radius should be dependent on avg luminance.
}

bool PointLight::cull() {
//	if(!Engine::frustumCulling) {
//		return false;
//	}
//
//	return !Shader::activeCamera->frustum.intersect(vec3(data.lightPosition), scale.x);
	return false;
}
