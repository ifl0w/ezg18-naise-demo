#include <systems/render-engine/textures/Texture.hpp>

using namespace NAISE::RenderCore;

Texture::Texture()
{
	/*glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	//glCompressedTexImage2D(GL_TEXTURE_2D, 0, image.format, image.width, image.height, 0, image.size, image.image);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);*/
}

void Texture::useTexture(uint32_t unit) {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture::setWrapping(GLenum DIRECTION, GLenum WRAPPING) {
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(TextureTarget::TARGET, DIRECTION, WRAPPING);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::setWrapping(GLenum WRAPPING) {
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(TextureTarget::TARGET, TextureWrappingDirection::DIRECTION_S, WRAPPING);
	glTexParameteri(TextureTarget::TARGET, TextureWrappingDirection::DIRECTION_T, WRAPPING);
	glBindTexture(GL_TEXTURE_2D, 0);
}
