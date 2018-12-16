#include <systems/render-engine/textures/GLTFTexture.hpp>
#include <Logger.hpp>

using namespace NAISE::RenderCore;

GLTFTexture::GLTFTexture(const tinygltf::Image& image, bool sRGB) {
	glBindTexture(GL_TEXTURE_2D, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	auto internalFormat = (sRGB) ? GL_SRGB : GL_RGB;
	auto format = GL_RGB;
	if (image.component == 3) {
		internalFormat = (sRGB) ? GL_SRGB : GL_RGB;
	} else if (image.component == 4) {
		internalFormat = (sRGB) ? GL_SRGB_ALPHA : GL_RGBA;
		format = GL_RGBA;
	} else {
		NAISE_ERROR_LOG("unknown image format");
	}

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, image.width, image.height, 0, format, GL_UNSIGNED_BYTE, image.image.data());

	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
}
