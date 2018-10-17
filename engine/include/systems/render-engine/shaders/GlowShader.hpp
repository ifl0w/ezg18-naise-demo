#pragma once

#include <systems/render-engine/materials/shaders/Shader.hpp>

namespace NAISE {
namespace Engine {

class GlowShader : public Shader {
public:
	GlowShader();

	void setTextureUnit(GLint textureHandle);
//	void setMSTextureUnit(GLint textureHandle);
	void setHorizontalUnit(bool horizontal);

private:
//	GLint isMultiSampledLocation = -1;
	GLint debugTexturePosition = -1;
//	GLint debugTextureMSLocation = -1;
	GLint horizontalLocation = -1;

};

}
}
