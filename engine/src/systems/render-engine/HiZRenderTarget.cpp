#include <systems/render-engine/HiZRenderTarget.hpp>
#include <sstream>
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>

using namespace NAISE::Engine;

HiZRenderTarget::HiZRenderTarget() = default;

HiZRenderTarget::HiZRenderTarget(int width, int height, int samples)
        : width(width),
          height(height),
          samples(samples) {

    samples = glm::max(1, samples);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glEnable(GL_MULTISAMPLE);

    glGenTextures(1, &gLinearDepth);
    glBindTexture(GL_TEXTURE_2D, gLinearDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_UNSIGNED_INT, nullptr);//GL_FLOAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gLinearDepth, 0);

    glGenRenderbuffers(1, &depth_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_rbo);

    GLenum attachments[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, attachments);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE) {
            spdlog::get("console")->debug("broken sampling");
        }
        throw std::runtime_error("NAISE::ENGINE::PostProcessingTarget >> Post processing render target creation failed.");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

HiZRenderTarget::~HiZRenderTarget() {
    glDeleteTextures(1, &gLinearDepth);
}

void HiZRenderTarget::use() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}
