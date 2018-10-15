#include <systems/render-engine/textures/SkyboxTexture.hpp>
#include <glbinding/gl/gl.h>
#include <iostream>
#include <spdlog/spdlog.h>
#include <Utils.hpp>

using namespace gl;
using namespace NAISE::RenderCore;

SkyboxTexture::SkyboxTexture() {

}

SkyboxTexture::SkyboxTexture(std::vector<NAISE::RenderCore::SkyboxImageData> data) {

	unsigned int tmpTextureID;
	glGenTextures(1, &tmpTextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tmpTextureID);

	for (unsigned int i = 0; i < data.size(); i++) {
		if (data[i].nrChannels == 3) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, data[i].width, data[i].height, 0, GL_RGB,
						 GL_UNSIGNED_BYTE, data[i].data);

		} else if(data[i].nrChannels == 4){
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, data[i].width, data[i].height, 0, GL_RGB,
						 GL_UNSIGNED_BYTE, data[i].data);
		} else if(data[i].nrChannels == 0){
			//ignore
			//Resources failed to load
			//Error handling where textures are loaded
		} else {
			NAISE_WARN_PERSISTENCE("Skybox::nrChannels unequal 3 or 4 is not supported for cubemaps; nrChannels: {}", data[i].nrChannels);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	//TODO generate Mipmaps
	textureID = tmpTextureID;
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

}

SkyboxTexture::~SkyboxTexture() {

}

void SkyboxTexture::useTexture(uint32_t unit) {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
}
