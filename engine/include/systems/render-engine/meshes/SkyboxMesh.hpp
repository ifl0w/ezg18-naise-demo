#pragma once

#include "Mesh.hpp"

namespace NAISE {
namespace RenderCore {

class SkyboxMesh :
		public Mesh {
public:
	SkyboxMesh(float width = 5, float height = 5, float depth = 5);
};

}
}

