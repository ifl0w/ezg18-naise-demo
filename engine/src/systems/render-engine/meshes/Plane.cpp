#include <systems/render-engine/meshes/Plane.hpp>

using namespace NAISE::Engine;

Plane::Plane(float width, float height) {
	vertices.insert(vertices.end(), {
		glm::vec3(-1 * width/2, 1 * height/2, 0),
		glm::vec3(-1 * width/2, -1 * height/2, 0),
		glm::vec3(1 * width/2, -1 * height/2, 0),
		glm::vec3(1 * width/2, 1 * height/2, 0)
	});

	indices.insert(indices.end(), {0, 1, 2, 0, 2, 3});
	normals.insert(normals.end(), {glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1)});
	uv_coords.insert(uv_coords.end(), {glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(1, 1)});

	computeTangentBasis();

	fillBuffers();
}
