#include <systems/render-engine/textures/SkyboxTexture.hpp>
#include <glbinding/gl/gl.h>
#include <iostream>
#include <spdlog/spdlog.h>

using namespace gl;
using namespace NAISE::RenderCore;

SkyboxTexture::SkyboxTexture(std::vector<NAISE::RenderCore::SkyboxImageData> data) {

	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	for (unsigned int i = 0; i < data.size(); i++) {
		if (data[0].nrChannels == 3) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, data[0].width, data[0].height, 0, GL_RGB,
						 GL_UNSIGNED_BYTE, data[0].data);

		} else if(data[0].nrChannels == 4){
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, data[0].width, data[0].height, 0, GL_RGB,
						 GL_UNSIGNED_BYTE, data[0].data);
		} else {
			spdlog::get("logger")->warn("nrChannels unequal 3 or 4 is not supported for cubemaps");
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	//TODO generate Mipmaps
	skyboxTextureID = textureID;
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

}

SkyboxTexture::~SkyboxTexture() {

}

void SkyboxTexture::useTexture(uint32_t unit) {
	glActiveTexture(GL_TEXTURE0 + unit);
	//glActiveTexture(TextureUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);
}
