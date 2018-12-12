#pragma once

#include <tiny_gltf.h>
#include "Texture.hpp"

//namespace tinygltf {
//class Image; // forward delclaration
//}

namespace NAISE {
namespace RenderCore {

class GLTFTexture:
		public Texture {
public:
	GLTFTexture(const tinygltf::Image& image, bool sRGB = false);

};

}
}

