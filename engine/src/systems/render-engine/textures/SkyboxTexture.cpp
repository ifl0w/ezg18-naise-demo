#include <systems/render-engine/textures/SkyboxTexture.hpp>
#include <glbinding/gl/gl.h>
#include <iostream>
#include <spdlog/spdlog.h>
#include <Logger.hpp>

using namespace gl;
using namespace NAISE::RenderCore;

SkyboxTexture::SkyboxTexture() {

}

SkyboxTexture::SkyboxTexture(std::vector<NAISE::RenderCore::SkyboxImageData> data) {

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0); //initial value for lowest defined mipmap level = 0

	int mipmapCount = 0;
	mipmapCount = (int)(log(fmax(data[0].width, data[0].height))/log(2));
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL,
					mipmapCount); //initial value for the highest defined mipmap level = 1000

	for (unsigned int i = 0; i < data.size(); i++) {
		if (data[i].nrChannels == 3) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, data[i].width, data[i].height, 0, GL_RGB,
						 GL_UNSIGNED_BYTE, data[i].data);

		} else if (data[i].nrChannels == 4) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB_ALPHA, data[i].width, data[i].height, 0, GL_RGB,
						 GL_UNSIGNED_BYTE, data[i].data);
		} else if (data[i].nrChannels == 0) {
			//ignore
			//Resources failed to load
			//Error handling where textures are loaded
		} else {
			NAISE_WARN_LOG("Skybox::nrChannels unequal 3 or 4 is not supported for cubemaps; nrChannels: {}",
								   data[i].nrChannels);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

}

SkyboxTexture::~SkyboxTexture() {

}

void SkyboxTexture::useTexture(uint32_t unit) {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
}
