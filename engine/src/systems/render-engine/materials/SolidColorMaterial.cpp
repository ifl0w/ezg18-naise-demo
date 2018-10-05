#include <systems/render-engine/materials/SolidColorMaterial.hpp>
#include <systems/render-engine/materials/shaders/SolidColorShader.hpp>
#include <Resources.hpp>

using namespace NAISE::Engine;

SolidColorMaterial::SolidColorMaterial()
		: SolidColorMaterial(glm::vec3(0, 0.5, 1)) {
}

SolidColorMaterial::SolidColorMaterial(glm::vec3 color)
		: color(color),
		  Material(true) {
	shader = Resources::getShader<SolidColorShader>();
	this->colorLocation = uniformLocation(shader->shaderID, "color");
//	this->useWatermeshAnimationLocation = uniformLocation(shader->shaderID, "useWatermeshAnimation");
}

void SolidColorMaterial::useMaterial() const {
	glUniform3fv(this->colorLocation, 1, glm::value_ptr(color));
//	glUniform1i(this->useWatermeshAnimationLocation, false);
}
