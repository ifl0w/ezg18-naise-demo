#include <sstream>
#include <utility>

#include <systems/render-engine/shaders/LightShader.hpp>

using namespace NAISE::Engine;

LightShader::LightShader(std::string vertexShaderPath, std::string fragmentShaderPath)
		: Shader(std::move(vertexShaderPath), std::move(fragmentShaderPath)) {
	this->positionBufferLocation = uniformLocation(shaderID, "gPosition");
	this->normalBufferLocation = uniformLocation(shaderID, "gNormal");
	this->albedoRoughnessBufferLocation = uniformLocation(shaderID, "gAlbedoRoughness");
	this->shadowMapLocation = uniformLocation(shaderID, "shadowMap");
	this->emissionMetallicBufferLocation = uniformLocation(shaderID, "gEmissionMetallic");
}

void LightShader::setLightProperties(const Light& light) {
//	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, 11, 0, sizeof(LightData));
//
//	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 11);
//	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(LightData), &light->data);
//	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	setLightProperty("lightPosition",  light.data.lightPosition);
	setLightProperty("direction", light.data.direction);
	setLightProperty("diffuse",  light.data.diffuse);
	setLightProperty("ambient",  light.data.ambient);
	setLightProperty("specular",  light.data.specular);
	setLightProperty("attConstant", light.data.attConstant);
	setLightProperty("attLinear", light.data.attLinear);
	setLightProperty("attQuadratic",  light.data.attQuadratic);
	setLightProperty("coneAngle",  light.data.coneAngle);
	setLightProperty("coneBlend",  light.data.coneBlend);
	setLightProperty("directional",  (bool) light.data.directional);
}


std::string LightShader::buildLightPropName(std::string propertyName) {
	std::ostringstream ss;
	ss << "light." << propertyName;
	return ss.str();
}

void LightShader::setLightProperty(std::string propertyName, vec4 value) {
	std::string name = buildLightPropName(propertyName);
	glUniform4fv(uniformLocation(shaderID, name), 1, glm::value_ptr(value));
}

void LightShader::setLightProperty(std::string propertyName, float value) {
	std::string name = buildLightPropName(propertyName);
	glUniform1f(uniformLocation(shaderID, name), value);
}

void LightShader::setLightProperty(std::string propertyName, bool value) {
	std::string name = buildLightPropName(propertyName);
	glUniform1i(uniformLocation(shaderID, name), value);
}
