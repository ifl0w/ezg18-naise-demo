#include <systems/render-engine/PostProcessingTarget.hpp>
#include <sstream>
#include <glm/gtc/matrix_transform.hpp>
#include <systems/render-engine/DeferredRenderTarget.hpp>
#include <spdlog/spdlog.h>

using namespace NAISE::Engine;

PostProcessingTarget::PostProcessingTarget() {
}

PostProcessingTarget::PostProcessingTarget(int width, int height, int samples)
		: width(width),
		  height(height),
		  samples(samples) {

	samples = glm::max(1, samples);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glEnable(GL_MULTISAMPLE);

	glGenTextures(1, &ping);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, ping);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGBA, width, height, GL_TRUE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, ping, 0);

	glGenTextures(1, &pong);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, pong);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGBA, width, height, GL_TRUE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D_MULTISAMPLE, pong, 0);

	unsigned int attachments[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, attachments);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE) {
			spdlog::get("console")->debug("broken sampling");
		}
		throw std::runtime_error("NAISE::ENGINE::DeferredRenderTarget >> Deferred render target creation failed.");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

PostProcessingTarget::~PostProcessingTarget() {
	glDeleteTextures(1, &ping);
	glDeleteTextures(1, &pong);
}

void PostProcessingTarget::use() {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}
