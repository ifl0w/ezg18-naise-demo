#pragma once

#include "../materials/shaders/Shader.hpp"

namespace NAISE {
namespace Engine {

class TextureDebugShader: public RenderCore::Shader {
public:
	TextureDebugShader();

	void setMSTextureUnit(GLint textureHandle);
	void setTextureUnit(GLint textureHandle);

private:
	GLint isMultiSampledLocation = -1;
	GLint debugTexturePosition = -1;
	GLint debugTextureMSLocation = -1;

};

}
}
