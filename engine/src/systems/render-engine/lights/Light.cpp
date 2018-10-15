#include <systems/render-engine/lights/Light.hpp>
#include <systems/render-engine/materials/shaders/Shader.hpp>

using namespace NAISE::RenderCore;

Light::Light()
		: Light(vec3(1), vec3(1), vec3(1), vec3(0.5), 1, 1, 1, 1, 1, vec3(0), false) {
}

Light::Light(vec3 position,
							   vec3 diffuse,
							   vec3 specular,
							   vec3 ambient,
							   float attConstant,
							   float attLinear,
							   float attQuadratic,
							   float coneAngle,
							   float coneBlend,
							   vec3 direction,
							   bool directional) {
	data.lightPosition = (vec4(0));
	data.diffuse = vec4(diffuse, 1);
	data.specular = vec4(specular, 1);
	data.ambient = vec4(ambient, 1);
	data.direction = vec4(direction, 1);
	data.attConstant = attConstant;
	data.attLinear = attLinear;
	data.attQuadratic = attQuadratic;
	data.coneAngle = coneAngle;
	data.coneBlend = coneBlend;
	data.directional = static_cast<uint32_t>(directional);

	position = position;
}

bool Light::cull() {
	return false;
}
