#include <systems/render-engine/shaders/TextureDebugShader.hpp>

using namespace NAISE::Engine;

TextureDebugShader::TextureDebugShader()
		: Shader("engine/shaders/deferred-shading/ds_debug.vert", "engine/shaders/deferred-shading/ds_debug.frag") {
	this->isMultiSampledLocation = uniformLocation(shaderID, "isMultiSampled");
	this->debugTexturePosition = uniformLocation(shaderID, "debugTexture");
	this->debugTextureMSLocation = uniformLocation(shaderID, "debugTextureMS");
}

void TextureDebugShader::setMSTextureUnit(GLint textureHandle) {
	glUniform1i(isMultiSampledLocation, GL_TRUE);
	glUniform1i(debugTextureMSLocation, 1);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureHandle);
}

void TextureDebugShader::setTextureUnit(GLint textureHandle) {
	glUniform1i(isMultiSampledLocation, GL_FALSE);
	glUniform1i(debugTexturePosition, 0);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, textureHandle);
}
