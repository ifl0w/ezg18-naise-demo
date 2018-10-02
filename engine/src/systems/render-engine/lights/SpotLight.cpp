#include "SpotLight.h"

SpotLight::SpotLight() {
}

SpotLight::SpotLight(vec3 position, vec3 color, vec3 direction, float angular, float blend)
		: Light(position, color, color, color, 1, 0, 0, angular, blend, direction, false) {
}

void SpotLight::setAttenuation(float constant, float linear, float quadratic) {
	data.attConstant = constant;
	data.attLinear = linear;
	data.attQuadratic = quadratic;
}