#include <components/MeshComponent.hpp>
#include <spdlog/spdlog.h>
#include <fstream>

using namespace NAISE::Engine;

MeshComponent::MeshComponent() {
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &positions_vbo);
	glGenBuffers(1, &indices_vbo);
	glGenBuffers(1, &normals_vbo);
	glGenBuffers(1, &uv_coords_vbo);
	glGenBuffers(1, &tangents_vbo);
	glGenBuffers(1, &instanceModelMatrix_vbo);
}

MeshComponent::~MeshComponent() {
	glDeleteBuffers(1, &tangents_vbo);
	glDeleteBuffers(1, &uv_coords_vbo);
	glDeleteBuffers(1, &normals_vbo);
	glDeleteBuffers(1, &indices_vbo);
	glDeleteBuffers(1, &positions_vbo);
	glDeleteBuffers(1, &instanceModelMatrix_vbo);
	glDeleteVertexArrays(1, &vao);
}

void MeshComponent::writeToObj(std::string filename) {
	std::ofstream outputFile;
	outputFile.open(filename);

	for (glm::vec3 v : vertices) {
		outputFile << "v " << v.x << " " << v.y << " " << v.z << std::endl;
	}

	for (glm::vec3 n : normals) {
		outputFile << "vn " << n.x << " " << n.y << " " << n.z << std::endl;
	}

	for (size_t i = 0; i < indices.size(); i += 3) {
		outputFile << "f " << indices[i] + 1 << " " << indices[i + 1] + 1 << " " << indices[i + 2] + 1 << std::endl;
	}

	outputFile.close();
}

std::vector<vec3> MeshComponent::getConvexHullData() {
	if (convexHullData.empty()) {
		spdlog::get("logger")->warn("NAISE::Engine::MeshComponent >> Convex hull is empty and should not be used.");
	}
	return convexHullData;
}

void MeshComponent::textureWrap(vec2 repeatTexture) {

}

void MeshComponent::computeTangentBasis() {
	tangents.resize(vertices.size());
}
