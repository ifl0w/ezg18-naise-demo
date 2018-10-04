#pragma once

#include <iostream>
#include <fstream>

#include "MeshFactory.hpp"

using namespace glm;

namespace NAISE {
namespace Engine {

class Box {
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
	static void generateFace(MeshComponent& c, vec3 v1, vec3 v2, vec3 v3, vec3 v4, vec3 normal);

	static void generateGeometry(MeshComponent& c, float width, float height, float depth);
};

}
}