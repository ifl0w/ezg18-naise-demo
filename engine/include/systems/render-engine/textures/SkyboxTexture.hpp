#pragma once

#include "Texture.hpp"
#include <vector>


namespace NAISE {
namespace RenderCore {

struct SkyboxImageData{
  int width, height, nrChannels;
  unsigned char* data;
};


class SkyboxTexture  :
		public Texture {
public:

	SkyboxTexture(std::vector<SkyboxImageData> data);

	~SkyboxTexture();

	void useTexture(uint32_t unit);

	GLuint skyboxTextureID = 0;

};
}
}
