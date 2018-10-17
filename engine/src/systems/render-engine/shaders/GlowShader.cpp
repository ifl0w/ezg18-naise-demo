#include <systems/render-engine/shaders/GlowShader.hpp>

using namespace NAISE::Engine;

GlowShader::GlowShader()
		: Shader("engine/shaders/deferred-shading/glow-shaders/glow.vert",
				 "engine/shaders/deferred-shading/glow-shaders/glow.frag") {
//	this->isMultiSampledLocation = uniformLocation(shaderID, "isMultiSampled");
	this->debugTexturePosition = uniformLocation(shaderID, "debugTexture");
//	this->debugTextureMSLocation = uniformLocation(shaderID, "debugTextureMS");
	this->horizontalLocation = uniformLocation(shaderID, "horizontal");
	GLint horizontal = -1;
}

/*void GlowShader::setMSTextureUnit(GLint textureHandle) {
	glUniform1i(isMultiSampledLocation, GL_TRUE);
//	glUniform1i(debugTextureMSLocation, 1);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureHandle);
}*/

void GlowShader::setHorizontalUnit(bool horizontal) {
	if (horizontal) {
		glUniform1i(horizontalLocation, GL_TRUE);
	} else {
		glUniform1i(horizontalLocation, GL_FALSE);
	}
}

void GlowShader::setTextureUnit(GLint textureHandle) {
	glUniform1i(debugTexturePosition, 4);
	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_2D, textureHandle);
}
