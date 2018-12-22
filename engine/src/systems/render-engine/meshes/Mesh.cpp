#include <systems/render-engine/meshes/Mesh.hpp>
#include <spdlog/spdlog.h>

#include "resource-loader/GLTFLoader.hpp"

using namespace NAISE::RenderCore;

Mesh::Mesh() {
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &positions_vbo);
	glGenBuffers(1, &indices_vbo);
	glGenBuffers(1, &normals_vbo);
	glGenBuffers(1, &uv_coords_vbo);
	glGenBuffers(1, &tangents_vbo);
	glGenBuffers(1, &instanceModelMatrix_vbo);
}

Mesh::~Mesh() {
	glDeleteBuffers(1, &tangents_vbo);
	glDeleteBuffers(1, &uv_coords_vbo);
	glDeleteBuffers(1, &normals_vbo);
	glDeleteBuffers(1, &indices_vbo);
	glDeleteBuffers(1, &positions_vbo);
	glDeleteBuffers(1, &instanceModelMatrix_vbo);
	glDeleteVertexArrays(1, &vao);
}

void Mesh::fillBuffers() {
	/* Bind vertex array as currently used one */
	glBindVertexArray(vao);

	/* Bind position_vbo as active buffer */
	glBindBuffer(GL_ARRAY_BUFFER, positions_vbo);
	/* Copy the vertex data to buffer */
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * 3 * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

	/* Enable attribute index 0 */
	glEnableVertexAttribArray(0);
	/* Coordinate data is going into attribute index 0 and contains three floats per vertex */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_vbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	if (normals.size() > 0) {
		/* Bind normals_vbo as active buffer */
		glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
		/* Copy the normals data to buffer */
		glBufferData(GL_ARRAY_BUFFER, normals.size() * 3 * sizeof(GLfloat), normals.data(), GL_STATIC_DRAW);

		/* Enable attribute index 1 */
		glEnableVertexAttribArray(1);
		/* Normal data is going into attribute index 1 and contains three floats per vertex */
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	if (uv_coords.size() > 0) {
		/* Bind uv_coords_vbo as active buffer */
		glBindBuffer(GL_ARRAY_BUFFER, uv_coords_vbo);
		/* Copy the uv data to buffer */
		glBufferData(GL_ARRAY_BUFFER, uv_coords.size() * 2 * sizeof(GLfloat), uv_coords.data(), GL_STATIC_DRAW);

		/* Enable attribute index 2 */
		glEnableVertexAttribArray(2);
		/* uv data is going into attribute index 2 and contains two floats per vertex */
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	}

	if (tangents.size() > 0) {
		/* Bind normals_vbo as active buffer */
		glBindBuffer(GL_ARRAY_BUFFER, tangents_vbo);
		/* Copy the normals data to buffer */
		glBufferData(GL_ARRAY_BUFFER, tangents.size() * 3 * sizeof(GLfloat), tangents.data(), GL_STATIC_DRAW);

		/* Enable attribute index 1 */
		glEnableVertexAttribArray(3);
		/* Normal data is going into attribute index 1 and contains three floats per vertex */
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}


	//Unbind VAO and VBOs
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

//	aabb = AABB(vertices);
}

Mesh::Mesh(std::vector<vec3> points): Mesh() {
	vertices.insert(vertices.end(), points.begin(), points.end());

	for (uint32_t i = 1; i < vertices.size(); ++i) {
		indices.push_back(i-1);
		indices.push_back(i);
	}
	indices.push_back(0);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, positions_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * 3 * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_vbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void Mesh::writeToObj(std::string filename) {
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
//
//void Mesh::draw() {
//	//Bind VAO
//	glBindVertexArray(vao);
//
//	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
//
//	//Unbind VAO
//	glBindVertexArray(0);
//}
//
//void Mesh::drawInstances(uint64_t numberInstances, GLuint ssboTransformations) {
//	glBindVertexArray(vao);
//
//	glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, numberInstances);
//
//	glBindVertexArray(0);
//}

std::vector<vec3> Mesh::getConvexHullData() {
	if (convexHullData.empty()) {
		spdlog::get("logger")->warn("NAISE::ENGINE::MESH :: Convex hull is empty and should not be used.");
	}
	return convexHullData;
}

void Mesh::textureWrap(vec2 repeatTexture) {

}

void Mesh::computeTangentBasis() {
	tangents.resize(vertices.size());
}

Mesh::Mesh(const tinygltf::Mesh& mesh, const tinygltf::Model& model): Mesh() {

	for (const auto& primitive: mesh.primitives) {
		// TODO: material

		// only supporting triangle primitives for now
		if (primitive.mode == TINYGLTF_MODE_TRIANGLES) {
			size_t idxOffset = vertices.size();

			for (const auto& attribute: primitive.attributes) {

				if (attribute.first == "POSITION") {
					std::vector<vec3> addVec = Engine::GLTFLoader::dataFromBuffer<glm::vec3>(attribute.second, model);
					vertices.insert(vertices.end(), addVec.begin(), addVec.end());
				} else if (attribute.first == "NORMAL") {
					std::vector<vec3> addVec = Engine::GLTFLoader::dataFromBuffer<glm::vec3>(attribute.second, model);
					normals.insert(normals.end(), addVec.begin(), addVec.end());
				} else if (attribute.first == "TEXCOORD_0") {
					std::vector<vec2> addVec = Engine::GLTFLoader::dataFromBuffer<glm::vec2>(attribute.second, model);
					uv_coords.insert(uv_coords.end(), addVec.begin(), addVec.end());
				} else if (attribute.first == "TANGENT") {
					std::vector<vec4> addVec = Engine::GLTFLoader::dataFromBuffer<glm::vec4>(attribute.second, model);
					tangents.insert(tangents.end(), addVec.begin(), addVec.end());
				}
			}

			std::vector<GLuint> addIdx = Engine::GLTFLoader::dataFromBuffer<GLuint>(primitive.indices, model);
			for (auto& idx: addIdx) {
				idx += idxOffset;
			}
			indices.insert(indices.end(),  addIdx.begin(), addIdx.end());
		} else {
			NAISE_ERROR_LOG("Requested primitive mode not supported.")
		}
	}

	fillBuffers();
}

Mesh::Mesh(const tinygltf::Primitive& primitive, const tinygltf::Model& model): Mesh() {
	// only supporting triangle primitives for now
	if (primitive.mode == TINYGLTF_MODE_TRIANGLES) {

		for (const auto& attribute: primitive.attributes) {

			if (attribute.first == "POSITION") {
				std::vector<vec3> addVec = Engine::GLTFLoader::dataFromBuffer<glm::vec3>(attribute.second, model);
				vertices.insert(vertices.end(), addVec.begin(), addVec.end());
			} else if (attribute.first == "NORMAL") {
				std::vector<vec3> addVec = Engine::GLTFLoader::dataFromBuffer<glm::vec3>(attribute.second, model);
				normals.insert(normals.end(), addVec.begin(), addVec.end());
			} else if (attribute.first == "TEXCOORD_0") {
				std::vector<vec2> addVec = Engine::GLTFLoader::dataFromBuffer<glm::vec2>(attribute.second, model);
				uv_coords.insert(uv_coords.end(), addVec.begin(), addVec.end());
			} else if (attribute.first == "TANGENT") {
				std::vector<vec4> addVec = Engine::GLTFLoader::dataFromBuffer<glm::vec4>(attribute.second, model);
				tangents.insert(tangents.end(), addVec.begin(), addVec.end());
			}
		}

		std::vector<GLuint> addIdx = Engine::GLTFLoader::dataFromBuffer<GLuint>(primitive.indices, model);
		indices.insert(indices.end(),  addIdx.begin(), addIdx.end());
	} else {
		NAISE_ERROR_LOG("Requested primitive mode not supported.")
	}

	fillBuffers();
}
