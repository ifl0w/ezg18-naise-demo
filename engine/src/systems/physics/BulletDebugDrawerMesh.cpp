#include <systems/physics/BulletDebugDrawerMesh.hpp>

using namespace NAISE::Engine;

BulletDebugDrawerMesh::BulletDebugDrawerMesh() {

}

void BulletDebugDrawerMesh::addLine(vec3 from, vec3 to) {
	vertices.insert(vertices.end(), { from, to });

	indices.insert(indices.end(), {static_cast<unsigned int>(vertices.size() - 2),
								   static_cast<unsigned int>(vertices.size()) - 1});
}

void BulletDebugDrawerMesh::finishDebugMesh() {
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, positions_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * 3 * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_vbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void BulletDebugDrawerMesh::startDebugFrame() {
	vertices.clear();
	indices.clear();
}

//void BulletDebugDrawerMesh::draw() {
//	glBindVertexArray(vao);
//	glDrawElements(GL_LINES, sizeOfLastIndexBuffer, GL_UNSIGNED_INT, nullptr);
//	glBindVertexArray(0);
//}

