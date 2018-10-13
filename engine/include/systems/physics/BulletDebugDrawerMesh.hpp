#pragma once


#include "../../meshes/Mesh.h"

namespace NAISE {
namespace ENGINE {

class BulletDebugDrawerMesh :
		  public Mesh {
public:
	BulletDebugDrawerMesh();

	void addLine(vec3 from, vec3 to);
	void finishDebugMesh();

	void draw() override;

private:
	uint32_t sizeOfLastIndexBuffer = 0;
};

}
}
