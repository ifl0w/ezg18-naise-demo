#include <systems/render-engine/RenderTarget.hpp>

using namespace NAISE::Engine;

RenderTarget::RenderTarget() {
	glGenFramebuffers(1, &fbo);
}

RenderTarget::~RenderTarget() {
	glDeleteFramebuffers(1, &fbo);
}
