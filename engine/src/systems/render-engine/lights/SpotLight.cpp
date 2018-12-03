#include <systems/render-engine/lights/SpotLight.hpp>

using namespace NAISE::RenderCore;

SpotLight::SpotLight() {
}

SpotLight::SpotLight(vec3 position, vec3 color, vec3 direction, float angular, float blend)
		: PointLight(position, color) {
	data.attConstant = 1;
	data.attLinear = 0;
	data.attQuadratic = 0;
	data.coneAngle = angular;
	data.coneBlend = blend;
	data.direction = vec4(direction, 0);
	data.directional = static_cast<uint32_t>(false);
}

//void SpotLight::setAttenuation(float constant, float linear, float quadratic) {
//	data.attConstant = constant;
//	data.attLinear = linear;
//	data.attQuadratic = quadratic;
//}