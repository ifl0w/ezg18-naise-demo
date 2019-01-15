#pragma once

#include "systems/render-engine/RenderTarget.hpp"

namespace NAISE {
namespace RenderCore {

class LightShader;

class ShadowMap: public NAISE::Engine::RenderTarget {
public:
	ShadowMap(int width, int height);

	void use() override;

	void setTextureUnits(const LightShader& lightShader);

	GLuint shadowMap;
};

}
}
