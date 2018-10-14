#pragma once

#include <iostream>
#include <fstream>

#include "Mesh.hpp"

namespace NAISE {
namespace RenderCore {

class Sphere :
		public Mesh {
public:
	Sphere(float radius = 1.0, unsigned int segments = 32, unsigned int rings = 16);

	float const radius;
	unsigned int const segments;
	unsigned int const rings;

private:
	void generateGeometry(float radius, unsigned int segments, unsigned int rings);

	void generateIndices(std::vector<GLuint>& geometryIndices);
};

}
}