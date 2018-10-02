#pragma once

#include "systems/render-engine/RenderTarget.hpp"
#include "systems/render-engine/shaders/LightShader.hpp"

namespace NAISE {
namespace Engine {

class ShadowMap: public RenderTarget {
public:
	ShadowMap(int width, int height);

	void use() override;

	void setTextureUnits(const LightShader& lightShader);

	int width;
	int height;

	GLuint shadowMap;
};

}
}
