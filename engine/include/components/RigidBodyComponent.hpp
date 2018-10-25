#pragma once

#include <memory>

#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <BulletCollision/CollisionShapes/btTriangleMesh.h>

#include "Component.hpp"

namespace NAISE {
namespace Engine {

class RigidBodyComponent: public Component {
public:
	std::unique_ptr<btRigidBody> rigidBody;
	std::unique_ptr<btMotionState> motionState;
	std::unique_ptr<btCollisionShape> collisionShape;

	std::unique_ptr<btTriangleMesh> triangleMesh;

	uint16_t collisionGroup = 0;
};

}
}
