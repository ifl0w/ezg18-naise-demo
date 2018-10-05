#pragma once

#include <systems/render-engine/materials/shaders/Shader.hpp>

namespace NAISE {
namespace Engine {

class LightShader: public Shader {
public:
	LightShader(std::string vertexShaderPath, std::string fragmentShaderPath);

	GLint shadowMapLocation = -1;
	GLint positionBufferLocation = -1;
	GLint normalBufferLocation = -1;
	GLint albedoRoughnessBufferLocation = -1;
	GLint emissionMetallicBufferLocation = -1;

	GLenum positionBufferUnit = GL_TEXTURE0 + 0;
	GLenum normalBufferUnit = GL_TEXTURE0 + 1;
	GLenum diffSpecBufferUnit = GL_TEXTURE0 + 2;
	GLenum shadowMapUnit = GL_TEXTURE0 + 3;
	GLenum glowBufferUnit = GL_TEXTURE0 + 4;

	void setLightProperties(const Light& light);

protected:
	std::string buildLightPropName(std::string propertyName);
	void setLightProperty(std::string propertyName, glm::vec4 value);
	void setLightProperty(std::string propertyName, float value);
	void setLightProperty(std::string propertyName, bool value);
};

}
}
