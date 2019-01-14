#pragma once

#include <systems/render-engine/materials/shaders/Shader.hpp>
#include <systems/render-engine/lights/Light.hpp>

namespace NAISE {
namespace RenderCore {

class LightShader: public Shader {
public:
	LightShader(std::string vertexShaderPath, std::string fragmentShaderPath);

	std::vector<GLint> shadowMapLocation = {-1, -1, -1};
	GLint positionBufferLocation = -1;
	GLint normalBufferLocation = -1;
	GLint albedoRoughnessBufferLocation = -1;
	GLint emissionMetallicBufferLocation = -1;

	GLenum positionBufferUnit = GL_TEXTURE0 + 0;
	GLenum normalBufferUnit = GL_TEXTURE0 + 1;
	GLenum diffSpecBufferUnit = GL_TEXTURE0 + 2;
	GLenum glowBufferUnit = GL_TEXTURE0 + 4;
	GLenum shadowMapUnit = GL_TEXTURE0 + 5;

	void setLightProperties(const NAISE::RenderCore::Light& light);

protected:
	std::string buildLightPropName(std::string propertyName);
	void setLightProperty(std::string propertyName, glm::vec4 value);
	void setLightProperty(std::string propertyName, float value);
	void setLightProperty(std::string propertyName, bool value);
};

}
}
