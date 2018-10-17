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
	glBindTexture(GL_TEXTURE_2D, ping);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ping, 0);

	glGenTextures(1, &pong);
	glBindTexture(GL_TEXTURE_2D, pong);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, pong, 0);

	GLenum attachments[1] = {GL_COLOR_ATTACHMENT0};
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
