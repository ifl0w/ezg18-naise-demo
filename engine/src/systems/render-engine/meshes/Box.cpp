#include "Box.hpp"

using namespace glm;

Box::Box(float width, float height, float depth)
		: width(width),
		  height(height),
		  depth(depth) {
	generateGeometry(width, height, depth);
	fillBuffers();
}

void Box::generateGeometry(float width, float height, float depth) {
	// front face
	vec3 v1 = glm::vec3(-0.5 * width, 0.5 * height, 0.5 * depth); // left upper front vertex
	vec3 v2 = v1 * glm::vec3(1, -1, 1);
	vec3 v3 = v1 * glm::vec3(-1, 1, 1);
	vec3 v4 = v1 * glm::vec3(-1, -1, 1);
	generateFace(v1, v2, v3, v4, vec3(0, 0, 1));

	// back face
	v1 = glm::vec3(0.5 * width, 0.5 * height, -0.5 * depth);
	v2 = v1 * glm::vec3(1, -1, 1);
	v3 = v1 * glm::vec3(-1, 1, 1);
	v4 = v1 * glm::vec3(-1, -1, 1);
	generateFace(v1, v2, v3, v4, vec3(0, 0, -1));

	// right face
	v1 = glm::vec3(0.5 * width, 0.5 * height, 0.5 * depth);
	v2 = v1 * glm::vec3(1, -1, 1);
	v3 = v1 * glm::vec3(1, 1, -1);
	v4 = v1 * glm::vec3(1, -1, -1);
	generateFace(v1, v2, v3, v4, vec3(1, 0, 0));

	// left face
	v1 = glm::vec3(-0.5 * width, 0.5 * height, -0.5 * depth);
	v2 = v1 * glm::vec3(1, -1, 1);
	v3 = v1 * glm::vec3(1, 1, -1);
	v4 = v1 * glm::vec3(1, -1, -1);
	generateFace(v1, v2, v3, v4, vec3(-1, 0, 0));

	// top face
	v1 = glm::vec3(-0.5 * width, 0.5 * height, -0.5 * depth);
	v2 = v1 * glm::vec3(1, 1, -1);
	v3 = v1 * glm::vec3(-1, 1, 1);
	v4 = v1 * glm::vec3(-1, 1, -1);
	generateFace(v1, v2, v3, v4, vec3(0, 1, 0));

	// bottom face
	v1 = glm::vec3(-0.5 * width, -0.5 * height, 0.5 * depth);
	v2 = v1 * glm::vec3(1, 1, -1);
	v3 = v1 * glm::vec3(-1, 1, 1);
	v4 = v1 * glm::vec3(-1, 1, -1);
	generateFace(v1, v2, v3, v4, vec3(0, -1, 0));

	computeTangentBasis();
}

void Box::generateFace(vec3 v1, vec3 v2, vec3 v3, vec3 v4, vec3 normal) {

	GLuint s = vertices.size();
	indices.insert(indices.end(), {s, s + 1, s + 3, s + 3, s + 2, s});

	vertices.insert(vertices.end(), {v1, v2, v3, v4});

	normals.insert(normals.end(), {normal, normal, normal, normal});

	uv_coords.insert(uv_coords.end(), {vec2(0, 1), vec2(0, 0), vec2(1, 1), vec2(1, 0)});

	// Edges of the triangle : position delta
	glm::vec3 deltaPos1 = v2 - v1;
	glm::vec3 deltaPos2 = v4 - v1;

	// UV delta
	glm::vec2 deltaUV1 = vec2(0, 0) - vec2(0, 1);
	glm::vec2 deltaUV2 = vec2(1, 0) - vec2(0, 1);

	float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
	glm::vec3 tangent = (deltaPos1 * deltaUV2.y   - deltaPos2 * deltaUV1.y)*r;
	tangents.push_back(tangent);
	tangents.push_back(tangent);

	// Edges of the triangle : position delta
	deltaPos1 = v3 - v4;
	deltaPos2 = v1 - v4;

	// UV delta
	deltaUV1 = vec2(1, 1) - vec2(1, 0);
	deltaUV2 = vec2(0, 1) - vec2(1, 0);

	r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
	tangent = (deltaPos1 * deltaUV2.y   - deltaPos2 * deltaUV1.y)*r;
	tangents.push_back(tangent);
	tangents.push_back(tangent);
}

void Box::textureWrap(vec2 repeatTexture) {
	uv_coords.clear();
	for(int i=0; i < 6; i++){
		uv_coords.insert(uv_coords.end(), {vec2(0, 1* repeatTexture.y), vec2(0, 0), vec2(1*repeatTexture.x, 1*repeatTexture.y), vec2(1*repeatTexture.x, 0)});
	}
	fillBuffers();
}

void Box::computeTangentBasis() {
}
