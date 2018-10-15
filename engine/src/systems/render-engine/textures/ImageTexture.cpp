#include <systems/render-engine/textures/ImageTexture.hpp>
#include <spdlog/spdlog.h>

using namespace NAISE::RenderCore;

ImageTexture ImageTexture::loadBMP(const char* imagepath) {
	// Data read from the header of the BMP file
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	unsigned int width, height;
	// Actual RGB data
	unsigned char* data;

	// Open the file
	FILE* file = fopen(imagepath, "rb");
	if (!file) {
		throw std::runtime_error("ImageTexture :: File could not be opened.");
	}

	// Read the header, i.e. the 54 first bytes

	// If less than 54 bytes are read, problem
	if (fread(header, 1, 54, file) != 54) {
		throw std::runtime_error("ImageTexture :: Not a correct BMP file.");
	}
	// A BMP files always begins with "BM"
	if (header[0] != 'B' || header[1] != 'M') {
		throw std::runtime_error("ImageTexture :: Not a correct BMP file.");
	}
	// Make sure this is a 24bpp file
	if (*(int*) &(header[0x1E]) != 0) {
		throw std::runtime_error("ImageTexture :: Not a correct BMP file.");
	}
	if (*(int*) &(header[0x1C]) != 24) {
		throw std::runtime_error("ImageTexture :: Not a correct BMP file.");
	}

	// Read the information about the image
	dataPos = *(int*) &(header[0x0A]);
	imageSize = *(int*) &(header[0x22]);
	width = *(int*) &(header[0x12]);
	height = *(int*) &(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize == 0) { imageSize = width * height * 3; } // 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0) { dataPos = 54; } // The BMP header is done that way

	// Create a buffer
	data = new unsigned char[imageSize];

	// Read the actual data from the file into the buffer
	fread(data, 1, imageSize, file);

	// Everything is in memory now, the file wan be closed
	fclose(file);

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	// OpenGL has now copied the data. Free our own version
	delete[] data;

	// Poor filtering, or ...
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// ... nice trilinear filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Return the ID of the texture we just created

	ImageTexture output;

	output.width = width;
	output.height = height;
	output.size = imageSize;
	output.data = data;
	output.format = GL_RGB;

	return output;
}

ImageTexture ImageTexture::loadDDS(const char* imagepath) {
	unsigned char header[124];

	FILE* fp;

	/* try to open the file */
	fp = fopen(imagepath, "rb");
	if (fp == NULL) {
		spdlog::get("logger")->error("NAISE::ENGINE::ImageTexture >> Could not open File: {}", imagepath);
		return ImageTexture();
	}

	/* verify the type of file */
	char filecode[4];
	fread(filecode, 1, 4, fp);
	if (strncmp(filecode, "DDS ", 4) != 0) {
		fclose(fp);
		spdlog::get("logger")->error("NAISE::ENGINE::ImageTexture >> File not in DDS format. ({})", imagepath);
		return ImageTexture();
	}

	/* get the surface desc */
	fread(&header, 124, 1, fp);

	unsigned int height = *(unsigned int*) &(header[8]);
	unsigned int width = *(unsigned int*) &(header[12]);
	unsigned int linearSize = *(unsigned int*) &(header[16]);
	unsigned int mipMapCount = *(unsigned int*) &(header[24]);
	unsigned int fourCC = *(unsigned int*) &(header[80]);

	unsigned char* buffer;
	unsigned int bufsize;
	/* how big is it going to be including all mipmaps? */
	bufsize = mipMapCount > 1 ? linearSize * 2
							  : linearSize; // NOTE: Convergence of the size of image pyramids is 2x input size
	buffer = (unsigned char*) malloc(
			bufsize * sizeof(unsigned char)); // will be freed by the destructor of the owning ImageTexture object
	fread(buffer, 1, bufsize, fp);
	/* close the file pointer */
	fclose(fp);

	unsigned int components = (fourCC == FOURCC_DXT1) ? 3 : 4;
	GLenum format;
	switch (fourCC) {
	case FOURCC_DXT1:
		format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		break;
	case FOURCC_DXT3:
		format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		break;
	case FOURCC_DXT5:
		format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		break;
	default:
		free(buffer);
		spdlog::get("logger")->error("NAISE::ENGINE::ImageTexture >> DDS format unknown. ({})", imagepath);
		return ImageTexture();
	}

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
	unsigned int offset = 0;

	/* load the mipmaps */
	if (mipMapCount < 1) {
		spdlog::get("logger")->warn("The file \"{}\" has no mip maps. Generating mip maps.", imagepath);
		unsigned int size = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;
		glCompressedTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, size, buffer + offset);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	for (unsigned int level = 0; level < mipMapCount && (width || height); ++level) {
		unsigned int size = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;
		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height, 0, size, buffer + offset);

		offset += size;
		width /= 2;
		height /= 2;

		// Deal with Non-Power-Of-Two textures. This code is not included in the webpage to reduce clutter.
		if (width < 1) { width = 1; }
		if (height < 1) { height = 1; }
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	ImageTexture output;

	output.width = width;
	output.height = height;
	output.size = linearSize; // only holds the first layer of the mip map
	output.data = buffer; // contains the whole data
	output.format = format;
	output.textureID = textureID;

	return output;
}
