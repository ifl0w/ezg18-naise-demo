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

	void bind(GLenum TextureUnit);

	GLuint skyboxTextureID = 0;

};
}
}
