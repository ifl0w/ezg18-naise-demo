#include <systems/render-engine/meshes/SkyboxMesh.hpp>

using namespace NAISE::RenderCore;

Skybox::Skybox(float width, float height, float depth) {

	auto x = width / 2;
	auto y = height / 2;
	auto z = depth / 2;

	vertices.insert(vertices.end(), {
			vec3(-1 * x, -1 * y, 1 * z),
			vec3(1 * x, -1 * y, 1 * z),
			vec3(1 * x, 1 * y, 1 * z),
			vec3(-1 * x, 1 * y, 1 * z),

			vec3(-1 * x, -1 * y, -1 * z),
			vec3(1 * x, -1 * y, -1 * z),
			vec3(1 * x, 1 * y, -1 * z),
			vec3(-1 * x, 1 * y, -1 * z)
	});

	indices.insert(indices.end(), {
			1, 0, 3, 2, 1, 3, // front
			0, 4, 7, 3, 0, 7, // left
			5, 1, 2, 6, 5, 2, // right
			5, 4, 0, 1, 5, 0, // bottom
			2, 3, 7, 6, 2, 7, // top
			6, 7, 4, 4, 5, 6 // back
	});

	indices.insert(indices.end(), {0, 1, 2, 0, 2, 3});
	fillBuffers();
}
