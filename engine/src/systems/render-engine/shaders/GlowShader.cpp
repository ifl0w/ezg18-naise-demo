#include <systems/render-engine/shaders/GlowShader.hpp>

using namespace NAISE::Engine;

GlowShader::GlowShader()
		: Shader("engine/shaders/deferred-shading/glow-shaders/glow.vert",
				 "engine/shaders/deferred-shading/glow-shaders/glow.frag") {

	this->debugTexturePosition = uniformLocation(this->shaderID, "emissionInput");
	this->horizontalLocation = uniformLocation(this->shaderID, "horizontal");
}

void GlowShader::setHorizontalUnit(bool horizontal) {
	if (horizontal) {
		glUniform1i(horizontalLocation, GL_TRUE);
	} else {
		glUniform1i(horizontalLocation, GL_FALSE);
	}
}

void GlowShader::setTextureUnit(GLint textureHandle) {
	glUniform1i(debugTexturePosition, 4);
	glActiveTexture(GL_TEXTURE0 + 4); // +4
	glBindTexture(GL_TEXTURE_2D, textureHandle);
}
