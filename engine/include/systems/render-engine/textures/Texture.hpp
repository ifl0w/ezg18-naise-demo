#pragma once

#include <glbinding/gl/gl.h>
#include <glm/glm.hpp>

using namespace gl;

namespace NAISE {
namespace RenderCore {

namespace TextureWrapping {
constexpr GLenum REPEAT = GL_REPEAT;
constexpr GLenum MIRRORED_REPEAT = GL_MIRRORED_REPEAT;
}

namespace TextureWrappingDirection {
constexpr GLenum DIRECTION_T = GL_TEXTURE_WRAP_T;
constexpr GLenum DIRECTION_S = GL_TEXTURE_WRAP_S;
constexpr GLenum DIRECTION_R = GL_TEXTURE_WRAP_R;
}

namespace TextureTarget { constexpr GLenum TARGET = GL_TEXTURE_2D; }

class Texture {
public:
	Texture();
	~Texture();

	explicit Texture(glm::ivec2 size,
			GLenum internalFormat = GL_RGBA16F,
			GLenum format = GL_RGBA,
			GLenum type = GL_FLOAT);

	GLuint textureID = 0;

	virtual void useTexture(uint32_t unit);

	void setWrapping(GLenum DIRECTION, GLenum WRAPPING); //bind and unbind
	void setWrapping(GLenum WRAPPING);
};

}
}