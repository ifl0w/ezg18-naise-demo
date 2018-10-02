#include <systems/render-engine/PingPongTarget.hpp>

using namespace NAISE::Engine;

PingPongTarget::PingPongTarget() {
	glGenFramebuffers(1, &fboPingPong);
	glBindFramebuffer(GL_FRAMEBUFFER, fboPingPong);
}

PingPongTarget::~PingPongTarget() {
	glDeleteFramebuffers(1, &fboPingPong);
}
