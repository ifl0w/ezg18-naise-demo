#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <glbinding/gl/gl.h>
//#include <tiny_gltf.h>
#include <glm/glm.hpp>

#include "Drawable.hpp"

using namespace glm;
using namespace gl;

namespace NAISE {
namespace Engine {

class Mesh :
		public Drawable {
public:
//	Mesh(const tinygltf::Mesh& mesh, const tinygltf::Model& model);
	Mesh();

	virtual ~Mesh();

	virtual void draw();
	virtual void drawInstances(uint64_t numberInstances, GLuint ssboTransformations);

	std::vector<vec3> getConvexHullData();

	virtual void writeToObj(std::string filename);

	virtual void textureWrap(vec2 repeatTexture);

	virtual void computeTangentBasis();

//	AABB aabb;

protected:
	std::vector<vec3> convexHullData;
	std::vector<vec3> vertices;
	std::vector<GLuint> indices;
	std::vector<vec3> normals;
	std::vector<vec2> uv_coords;
	std::vector<vec3> tangents;

	void fillBuffers();

	GLuint vao; // each mesh contains one VAO

	GLuint positions_vbo; // each mesh contains one position_vbo
	GLuint indices_vbo; // each mesh contains one index_vbo
	GLuint normals_vbo; // each mesh contains one normals_vbo
	GLuint uv_coords_vbo; // each mesh contains one uv_coords_vbo
	GLuint tangents_vbo; // each mesh contains one tangents_vbo

	GLuint instanceModelMatrix_vbo; // each mesh contains one uv_coords_vbo

//private:
//	std::vector<vec3> vec3FromGLTFBuffer(int accessorIdx, const tinygltf::Model& model);
//	std::vector<vec2> vec2FromGLTFBuffer(int accessorIdx, const tinygltf::Model& model);
//	std::vector<GLuint> gluintFromGLTFBuffer(int accessorIdx, const tinygltf::Model& model);
};

}
}
