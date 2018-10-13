#pragma once

#include "Mesh.hpp"

namespace NAISE {
namespace RenderCore {

class Skybox :
		public Mesh {
public:
	Skybox(float width = 5, float height = 5, float depth = 5);
};

}
}

