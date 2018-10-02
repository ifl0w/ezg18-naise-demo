#pragma once

#include "RenderTarget.hpp"
#include "systems/render-engine/shaders/LightShader.hpp"
#include "systems/render-engine/shaders/NullShader.hpp"
#include "systems/render-engine/shaders/DirectionalLightShader.hpp"

namespace NAISE {
namespace Engine {

class DeferredRenderTarget: public RenderTarget {
public:
	DeferredRenderTarget();
	DeferredRenderTarget(int width, int height, int samples);
	~DeferredRenderTarget() override;

	void use() override;

	void setTextureUnits(const LightShader& lightShader);
//	void setTextureUnits(const std::unique_ptr<DirectionalLightShader>& lightShader);

	void retrieveDepthBuffer();

	GLuint gGlow;
	GLuint gPosition;
	GLuint gNormal;
	GLuint gAlbedoSpec;
	GLuint depth_rbo;
private:
	int width;
	int height;
	int samples;
};

}
}
