#include <meshes/MeshFactory.hpp>
#include <memory>
#include <meshes/Box.hpp>
#include <meshes/Sphere.hpp>

using namespace NAISE::Engine;

void MeshFactory::fillBuffers(MeshComponent& c) {
	/* Bind vertex array as currently used one */
	glBindVertexArray(c.vao);

	/* Bind position_vbo as active buffer */
	glBindBuffer(GL_ARRAY_BUFFER, c.positions_vbo);
	/* Copy the vertex data to buffer */
	glBufferData(GL_ARRAY_BUFFER, c.vertices.size() * 3 * sizeof(GLfloat), c.vertices.data(), GL_STATIC_DRAW);

	/* Enable attribute index 0 */
	glEnableVertexAttribArray(0);
	/* Coordinate data is going into attribute index 0 and contains three floats per vertex */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, c.indices_vbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, c.indices.size() * sizeof(GLuint), c.indices.data(), GL_STATIC_DRAW);

	/* Bind normals_vbo as active buffer */
	glBindBuffer(GL_ARRAY_BUFFER, c.normals_vbo);
	/* Copy the normals data to buffer */
	glBufferData(GL_ARRAY_BUFFER, c.normals.size() * 3 * sizeof(GLfloat), c.normals.data(), GL_STATIC_DRAW);

	/* Enable attribute index 1 */
	glEnableVertexAttribArray(1);
	/* Normal data is going into attribute index 1 and contains three floats per vertex */
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	if (c.uv_coords.size() > 0) {
		/* Bind uv_coords_vbo as active buffer */
		glBindBuffer(GL_ARRAY_BUFFER, c.uv_coords_vbo);
		/* Copy the uv data to buffer */
		glBufferData(GL_ARRAY_BUFFER, c.uv_coords.size() * 2 * sizeof(GLfloat), c.uv_coords.data(), GL_STATIC_DRAW);

		/* Enable attribute index 2 */
		glEnableVertexAttribArray(2);
		/* uv data is going into attribute index 2 and contains two floats per vertex */
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	}

	if (c.tangents.size() > 0) {
		/* Bind normals_vbo as active buffer */
		glBindBuffer(GL_ARRAY_BUFFER, c.tangents_vbo);
		/* Copy the normals data to buffer */
		glBufferData(GL_ARRAY_BUFFER, c.tangents.size() * 3 * sizeof(GLfloat), c.tangents.data(), GL_STATIC_DRAW);

		/* Enable attribute index 1 */
		glEnableVertexAttribArray(3);
		/* Normal data is going into attribute index 1 and contains three floats per vertex */
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	}


	//Unbind VAO and VBOs
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// deprecated
	// c.aabb = AABB(c.vertices);
}


shared_ptr<MeshComponent> MeshFactory::createBox(float width, float height, float depth) {
	auto boxMesh = std::make_shared<MeshComponent>();

	Box::generateGeometry(*boxMesh.get(), width, height, depth);
	MeshFactory::fillBuffers(*boxMesh.get());

	return boxMesh;
}

shared_ptr<MeshComponent> MeshFactory::createSphere(float radius, uint32_t segments, uint32_t rings) {
	auto boxMesh = std::make_shared<MeshComponent>();

	unsigned int vertexCount = (rings + 1) * (segments + 1);
	boxMesh->vertices.resize(vertexCount);
	boxMesh->normals.resize(vertexCount);
	boxMesh->uv_coords.resize(vertexCount);

	Sphere::generateGeometry(*boxMesh.get(), radius, segments, rings);
	Sphere::generateIndices(*boxMesh.get(), boxMesh->indices, segments, rings, boxMesh->indices);

	MeshFactory::fillBuffers(*boxMesh.get());

	return boxMesh;
}