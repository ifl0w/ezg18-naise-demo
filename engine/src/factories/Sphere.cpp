#include <factories/Sphere.hpp>

using namespace glm;
using namespace NAISE::Engine;

void Sphere::generateGeometry(MeshComponent& c, float radius, unsigned int segments, unsigned int rings) {
	float azimuthalAngle = radians(360.0f / segments); // counter clockwise rotation
	float polarAngle = radians(180.0f / rings); // counter clockwise rotation

	float azimuthUVStep = 1.0f / segments;
	float polarUVStep = 1.0f / rings;

	unsigned int numVerticesLateral = segments * 2;
	unsigned int segmentsUV = segments + 1;
	for (unsigned int i = 0; i < rings + 1; i++) {

		unsigned int currentIdx = 0;
		for (size_t j = 0; j < segmentsUV; j++) {
			currentIdx = segmentsUV * i + j;

			float x = radius * sin((i) * polarAngle) * cos(j * azimuthalAngle);
			float y = radius * sin((i) * polarAngle) * sin(j * azimuthalAngle);
			float z = radius * cos((i) * polarAngle);

			c.mesh->vertices[currentIdx] = vec3(x, z, y); // also swapped z and y
			c.mesh->normals[currentIdx] = normalize(vec3(x, z, y));
			c.mesh->uv_coords[currentIdx] = vec2(j * azimuthUVStep, i * polarUVStep);

			if (i > 0) { // don't add indices on first loop
				c.mesh->indices.insert(c.mesh->indices.end(), {currentIdx, currentIdx - segmentsUV});
			}
		}

		if (i > 0) { // don't add indices on first loop
			// close current triangle strip
			// not needed since UV mapping (lateral surface stays open)
			//indices.insert(indices.end(), { currentIdx + 1 - segmentsUV, currentIdx + 1 - 2 * segmentsUV });
		}
	}
}

void Sphere::generateIndices(MeshComponent& c, std::vector<GLuint>& geometryIndices, uint32_t segments, uint32_t rings, std::vector<GLuint>& indices) {
	std::vector<GLuint> tmp_indices;
	unsigned int size = (segments + 1) * 2;

	for (size_t i = 2; i < size; i++) {
		tmp_indices.insert(tmp_indices.end(), {indices[i - 2], indices[i - 1], indices[i]});
	}

	for (size_t i = 1; i < rings - 1; i++) {
		for (size_t j = i * size + 2; j < i * size + size; j += 2) {
			tmp_indices.insert(tmp_indices.end(), {indices[j - 2], indices[j - 1], indices[j]});
			tmp_indices.insert(tmp_indices.end(), {indices[j + 1], indices[j], indices[j - 1]});
		}
	}

	size_t topFanIdx = (rings - 1) * size;
	for (size_t i = topFanIdx + 2; i < topFanIdx + size; i++) {
		tmp_indices.insert(tmp_indices.end(), {indices[i], indices[i - 1], indices[i - 2]});
	}

	geometryIndices = tmp_indices;
}
