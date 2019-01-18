#include <systems/render-engine/RenderTarget.hpp>

using namespace NAISE::Engine;

RenderTarget::RenderTarget() : RenderTarget(1024, 768, 0) {}

RenderTarget::RenderTarget(int width, int height, int samples): width(width), height(height), samples(samples) {
	glGenFramebuffers(1, &fbo);
}

RenderTarget::~RenderTarget() {
	glDeleteFramebuffers(1, &fbo);
}

void RenderTarget::retrieveDepthBuffer(RenderTarget* target) {
	retrieveDepthBuffer(target->fbo);
}

void RenderTarget::retrieveDepthBuffer(GLuint target_fbo) {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, this->fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target_fbo); // write to default framebuffer
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}
