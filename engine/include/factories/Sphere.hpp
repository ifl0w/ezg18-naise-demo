#pragma once

#include <iostream>
#include <fstream>

#include "MeshFactory.hpp"

using namespace glm;

namespace NAISE {
namespace Engine {

class Sphere {
private:
	friend MeshFactory;

	/*
		Insert vertecies, indices and normals.
		Vertex parameters have to be defined in the correct order:
			v1 = left upper
			v2 = left lower
			v3 = right upper
			v4 = right lower
	*/

private:
	static void generateGeometry(MeshComponent& c, float radius, unsigned int segments, unsigned int rings);

	static void generateIndices(MeshComponent& c, std::vector<GLuint>& geometryIndices, uint32_t segments, uint32_t rings, std::vector<GLuint>& indices);
};

}
}