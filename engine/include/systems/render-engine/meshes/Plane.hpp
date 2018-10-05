#pragma once

#include "Mesh.hpp"

namespace NAISE {
namespace RenderCore {

class Plane :
		public Mesh {
public:
	Plane(float width, float height);
};

}
}
