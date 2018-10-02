#pragma once

#import "../materials/shaders/Shader.hpp"

namespace NAISE {
namespace Engine {

class TextureDebugShader: public Shader {
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
