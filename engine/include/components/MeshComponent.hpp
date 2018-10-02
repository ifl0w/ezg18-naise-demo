#pragma once

#include "Component.hpp"

#include <vector>
#include <glm/glm.hpp>
#include <glbinding/gl/gl.h>

using namespace glm;
using namespace gl;
using namespace std;

namespace NAISE {
namespace Engine {

class MeshComponent: public Component {
public:
	MeshComponent();
	~MeshComponent();

	vector<vec3> convexHullData;
	vector<vec3> vertices;
	vector<GLuint> indices;
	vector<vec3> normals;
	vector<vec2> uv_coords;
	vector<vec3> tangents;

	GLuint vao; // each mesh contains one VAO

	GLuint positions_vbo; // each mesh contains one position_vbo
	GLuint indices_vbo; // each mesh contains one index_vbo
	GLuint normals_vbo; // each mesh contains one normals_vbo
	GLuint uv_coords_vbo; // each mesh contains one uv_coords_vbo
	GLuint tangents_vbo; // each mesh contains one tangents_vbo

	/**
	 * The model matrices for the mesh instances.
	 */
	GLuint instanceModelMatrix_vbo; // each mesh contains one instanceModelMatrix_vbo

	vector<vec3> getConvexHullData();

	virtual void writeToObj(std::string filename);

	virtual void textureWrap(vec2 repeatTexture);

	virtual void computeTangentBasis();

	// AABB aabb;
};

}
}
