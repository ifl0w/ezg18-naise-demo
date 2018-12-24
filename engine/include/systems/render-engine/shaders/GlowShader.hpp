#pragma once

#include <systems/render-engine/materials/shaders/Shader.hpp>

namespace NAISE {
namespace Engine {

class GlowShader : public NAISE::RenderCore::Shader {
public:
	GlowShader();

	void setTextureUnit(GLint textureHandle);

	void setHorizontalUnit(bool horizontal);

private:

	GLint debugTexturePosition = -1;
	GLint horizontalLocation = -1;

};

}
}
