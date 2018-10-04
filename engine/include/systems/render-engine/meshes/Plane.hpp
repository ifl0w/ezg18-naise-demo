#pragma once

#include "Mesh.hpp"

namespace NAISE {
namespace RenderEngine {

class Plane :
		public Mesh {
public:
	Plane(float width, float height);
};

}
}
