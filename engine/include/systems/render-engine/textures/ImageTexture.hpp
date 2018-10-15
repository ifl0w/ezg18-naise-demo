#pragma once

#include "Texture.hpp"

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

namespace NAISE {
namespace RenderCore {

class ImageTexture :
		public Texture {
public:
	unsigned char* data;
	unsigned int width;
	unsigned int height;
	unsigned int size;
	GLenum format;

	ImageTexture()
			: data(nullptr),
			  width(0),
			  height(0),
			  size(0),
			  format(GL_NONE) {
		textureID = 0;
	}

	ImageTexture(const ImageTexture& img) = delete;

	ImageTexture(ImageTexture&& img) noexcept
			: data(img.data),
			  width(img.width),
			  height(img.height),
			  size(img.size),
			  format(img.format) {
		img.data = nullptr;

		textureID = img.textureID;
		img.textureID = 0;
	}

	ImageTexture& operator=(const ImageTexture& img) = delete;

	ImageTexture& operator=(ImageTexture&& img) noexcept {
		data = img.data;
		img.data = nullptr;
		textureID = img.textureID;
		width = img.width;
		height = img.height;
		size = img.size;
		format = img.format;
		return *this;
	};

	~ImageTexture() {
		if (data != nullptr) {
			delete[] data;
			data = nullptr;
		}

		if (textureID != 0) {
			glBindTexture(GL_TEXTURE_2D, 0);
			glDeleteTextures(1, &textureID);
		}
	}

	static ImageTexture loadDDS(const char* file);

	static ImageTexture loadBMP(const char* file);
};

}
}