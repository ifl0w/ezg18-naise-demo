#include <factories/MeshFactory.hpp>
#include <memory>
#include <factories/Box.hpp>
#include <factories/Sphere.hpp>

using namespace NAISE::Engine;

void MeshFactory::fillBuffers(MeshComponent& c) {
	/* Bind vertex array as currently used one */
	glBindVertexArray(c.mesh->vao);

	/* Bind position_vbo as active buffer */
	glBindBuffer(GL_ARRAY_BUFFER, c.mesh->positions_vbo);
	/* Copy the vertex data to buffer */
	glBufferData(GL_ARRAY_BUFFER, c.mesh->vertices.size() * 3 * sizeof(GLfloat), c.mesh->vertices.data(), GL_STATIC_DRAW);

	/* Enable attribute index 0 */
	glEnableVertexAttribArray(0);
	/* Coordinate data is going into attribute index 0 and contains three floats per vertex */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, c.mesh->indices_vbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, c.mesh->indices.size() * sizeof(GLuint), c.mesh->indices.data(), GL_STATIC_DRAW);

	/* Bind normals_vbo as active buffer */
	glBindBuffer(GL_ARRAY_BUFFER, c.mesh->normals_vbo);
	/* Copy the normals data to buffer */
	glBufferData(GL_ARRAY_BUFFER, c.mesh->normals.size() * 3 * sizeof(GLfloat), c.mesh->normals.data(), GL_STATIC_DRAW);

	/* Enable attribute index 1 */
	glEnableVertexAttribArray(1);
	/* Normal data is going into attribute index 1 and contains three floats per vertex */
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	if (c.mesh->uv_coords.size() > 0) {
		/* Bind uv_coords_vbo as active buffer */
		glBindBuffer(GL_ARRAY_BUFFER, c.mesh->uv_coords_vbo);
		/* Copy the uv data to buffer */
		glBufferData(GL_ARRAY_BUFFER, c.mesh->uv_coords.size() * 2 * sizeof(GLfloat), c.mesh->uv_coords.data(), GL_STATIC_DRAW);

		/* Enable attribute index 2 */
		glEnableVertexAttribArray(2);
		/* uv data is going into attribute index 2 and contains two floats per vertex */
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	}

	if (c.mesh->tangents.size() > 0) {
		/* Bind normals_vbo as active buffer */
		glBindBuffer(GL_ARRAY_BUFFER, c.mesh->tangents_vbo);
		/* Copy the normals data to buffer */
		glBufferData(GL_ARRAY_BUFFER, c.mesh->tangents.size() * 3 * sizeof(GLfloat), c.mesh->tangents.data(), GL_STATIC_DRAW);

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
	// c.mesh->aabb = AABB(c.mesh->vertices);
}


shared_ptr<MeshComponent> MeshFactory::createBox(float width, float height, float depth) {
	auto boxMesh = std::make_shared<MeshComponent>();

	Box::generateGeometry(*boxMesh.get(), width, height, depth);
	MeshFactory::fillBuffers(*boxMesh.get());

	return boxMesh;
}

shared_ptr<MeshComponent> MeshFactory::createSphere(float radius, uint32_t segments, uint32_t rings) {
	auto comp = std::make_shared<MeshComponent>();

	unsigned int vertexCount = (rings + 1) * (segments + 1);
	comp->mesh->vertices.resize(vertexCount);
	comp->mesh->normals.resize(vertexCount);
	comp->mesh->uv_coords.resize(vertexCount);

	Sphere::generateGeometry(*comp.get(), radius, segments, rings);
	Sphere::generateIndices(*comp.get(), comp->mesh->indices, segments, rings, comp->mesh->indices);

	MeshFactory::fillBuffers(*comp.get());

	return comp;
}