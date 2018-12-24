#include <systems/render-engine/shadow-map/ShadowMap.hpp>
#include <systems/render-engine/shaders/LightShader.hpp>
#include <spdlog/spdlog.h>

using namespace NAISE::RenderCore;

ShadowMap::ShadowMap(int width, int height): width(width), height(height) {
	glGenTextures(1, &shadowMap);

	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(glm::vec4(1)));
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);

	// Disable writes to the color buffer
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		spdlog::get("logger")->error("NAISE::ENGINE::ShadowMap >> Shadow map creation failed! (Status {})", (int) status);
	}
}

void ShadowMap::use() {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void ShadowMap::setTextureUnits(const LightShader& lightShader) {
	glUniform1i(lightShader.shadowMapLocation, 3);
	glActiveTexture(lightShader.shadowMapUnit);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
}
