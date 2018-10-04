#pragma once

#include "Component.hpp"

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glbinding/gl/gl.h>

#include <systems/render-engine/meshes/Mesh.hpp>

using namespace glm;
using namespace gl;
using namespace std;
using namespace NAISE::RenderEngine;

namespace NAISE {
namespace Engine {

class MeshComponent: public Component {
public:
	MeshComponent(): mesh(make_shared<Mesh>()) {};

	shared_ptr<Mesh> mesh;

	// AABB aabb;
};

}
}
