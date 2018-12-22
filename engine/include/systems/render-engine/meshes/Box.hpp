#pragma once

#include <iostream>
#include <fstream>

#include "Mesh.h"

using namespace glm;

namespace NAISE {
namespace RenderCore {

class Box : public Mesh {
public:
	Box(float width, float height, float depth);
	Box(vec3 width, float height, float depth);

	const float width;
	const float height;
	const float depth;

	virtual void textureWrap(vec2 repeatTexture) override;
	virtual void computeTangentBasis() override;

private:
	/*
		Insert vertecies, indices and normals.
		Vertex parameters have to be defined in the correct order:
			v1 = left upper
			v2 = left lower
			v3 = right upper
			v4 = right lower
	*/
	void generateFace(vec3 v1, vec3 v2, vec3 v3, vec3 v4, vec3 normal);

	void generateGeometry(float width, float height, float depth);
};

}
}