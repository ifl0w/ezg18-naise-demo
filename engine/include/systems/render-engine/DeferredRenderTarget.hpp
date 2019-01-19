#pragma once

#include "RenderTarget.hpp"
#include "systems/render-engine/shaders/LightShader.hpp"
#include "systems/render-engine/shaders/NullShader.hpp"
//#include "systems/render-engine/shaders/DirectionalLightShader.hpp"

namespace NAISE {
namespace Engine {

class DeferredRenderTarget: public RenderTarget {
public:
	DeferredRenderTarget();
	DeferredRenderTarget(int width, int height, int samples);
	~DeferredRenderTarget() override;

	void use() override;

	void setTextureUnits(const NAISE::RenderCore::LightShader& lightShader);
//	void setTextureUnits(const std::unique_ptr<DirectionalLightShader>& lightShader);

	GLuint gEmissionMetallic;
	GLuint gPosition;
	GLenum gPositionAttachment = GL_COLOR_ATTACHMENT0;
	GLuint gNormal;
	GLuint gAlbedoRoughness;
	GLuint gLinearDepth;
	GLenum gLinearDepthAttachment = GL_COLOR_ATTACHMENT4;
	GLuint depth_rbo;

};

}
}
