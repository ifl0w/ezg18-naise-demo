#pragma once

#include "Mesh.hpp"

namespace NAISE {
namespace Engine {

class Plane :
		public Mesh {
public:
	Plane(float width, float height);
};

}
}
