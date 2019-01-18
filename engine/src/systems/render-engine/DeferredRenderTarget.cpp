#include <sstream>
#include <glm/gtc/matrix_transform.hpp>
#include <systems/render-engine/DeferredRenderTarget.hpp>

#include <systems/render-engine/lights/Light.hpp>
#include <systems/render-engine/meshes/Sphere.hpp>
#include <spdlog/spdlog.h>

using namespace NAISE::Engine;
using namespace NAISE::RenderCore;

DeferredRenderTarget::DeferredRenderTarget() = default;

DeferredRenderTarget::DeferredRenderTarget(int width, int height, int samples): RenderTarget(width, height, samples)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glEnable(GL_MULTISAMPLE);

	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0,  GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, gPositionAttachment, GL_TEXTURE_2D, gPosition, 0);

	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	glGenTextures(1, &gAlbedoRoughness);
	glBindTexture(GL_TEXTURE_2D, gAlbedoRoughness);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoRoughness, 0);

	glGenTextures(1, &gEmissionMetallic);
	glBindTexture(GL_TEXTURE_2D, gEmissionMetallic);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr); //TODO 16F
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gEmissionMetallic, 0);

    glGenTextures(1, &gLinearDepth);
    glBindTexture(GL_TEXTURE_2D, gLinearDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, gLinearDepth, 0);

    // depth texture is gonna be a mipmap so we have to establish the mipmap chain
   // glGenerateMipmap(GL_TEXTURE_2D);

	glGenRenderbuffers(1, &depth_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_rbo);

    /*GLenum attachments[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
    glDrawBuffers(4, attachments);*/
	GLenum attachments[5] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4};
	glDrawBuffers(5, attachments);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE) {
			spdlog::get("console")->debug("broken sampling");
		}
		throw std::runtime_error("NAISE::ENGINE::DeferredRenderTarget >> Deferred render target creation failed.");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

DeferredRenderTarget::~DeferredRenderTarget() {
	glDeleteTextures(1, &gPosition);
	glDeleteTextures(1, &gNormal);
	glDeleteTextures(1, &gAlbedoRoughness);
	glDeleteTextures(1, &gEmissionMetallic);
	glDeleteTextures(1, &gLinearDepth);
}

void DeferredRenderTarget::use() {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void DeferredRenderTarget::setTextureUnits(const LightShader& lightShader) {
	glUniform1i(lightShader.positionBufferLocation, 0);
	glActiveTexture(lightShader.positionBufferUnit);
	glBindTexture(GL_TEXTURE_2D, gPosition);

	glUniform1i(lightShader.normalBufferLocation, 1);
	glActiveTexture(lightShader.normalBufferUnit);
	glBindTexture(GL_TEXTURE_2D, gNormal);

	glUniform1i(lightShader.albedoRoughnessBufferLocation, 2);
	glActiveTexture(lightShader.diffSpecBufferUnit);
	glBindTexture(GL_TEXTURE_2D, gAlbedoRoughness);

	glUniform1i(lightShader.emissionMetallicBufferLocation, 4);
	glActiveTexture(lightShader.glowBufferUnit);
	glBindTexture(GL_TEXTURE_2D, gEmissionMetallic);
}
