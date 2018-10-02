#include "PointLight.h"

#include "../cameras/Camera.h"
#include "../Engine.h"
#include "../materials/shaders/Shader.h"

PointLight::PointLight()
		: PointLight(vec3(0)) {
}

PointLight::PointLight(vec3 position)
		: PointLight(position, vec3(1)) {
}

PointLight::PointLight(vec3 position, vec3 color)
		: Light(position, color, color, color, 1.0, 0.7, 1.8, 360, 0, vec3(0), false) {

	this->scale = vec3(calculateLightVolumeRadius());
}

void PointLight::setAttenuation(float constant, float linear, float quadratic) {
	data.attConstant = constant;
	data.attLinear = linear;
	data.attQuadratic = quadratic;

	this->scale = vec3(calculateLightVolumeRadius());
}

float PointLight::calculateLightVolumeRadius() {
	float constant = data.attConstant;
	float linear = data.attLinear;
	float quadratic = data.attQuadratic;

	float minLightValue = 5.0f / 256.0f;

	float lightMax = glm::max(glm::max(data.diffuse.r, data.diffuse.g), data.diffuse.b);
	double radius = (-linear + glm::sqrt(linear * linear - 4 * quadratic * (constant - (glm::pow(minLightValue, -1)) * lightMax)))
			/ (2 * quadratic);

	return static_cast<float>(radius);
}

bool PointLight::cull() {
	if(!Engine::frustumCulling) {
		return false;
	}

	return !Shader::activeCamera->frustum.intersect(vec3(data.lightPosition), scale.x);
}
