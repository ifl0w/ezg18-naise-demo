#pragma once

#include <systems/render-engine/meshes/Mesh.hpp>

namespace NAISE {
namespace Engine {

class BulletDebugDrawerMesh : public RenderCore::Mesh {
public:
	BulletDebugDrawerMesh();

	void addLine(vec3 from, vec3 to);

	void startDebugFrame();
	void finishDebugMesh();
};

}
}
