#pragma once

#include <memory>

#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <BulletCollision/CollisionShapes/btTriangleMesh.h>
#include <BulletCollision/CollisionShapes/btConvexHullShape.h>

#include "Component.hpp"

namespace NAISE {
namespace Engine {

class RigidBodyComponent: public Component {
public:
	std::unique_ptr<btRigidBody> rigidBody;
	std::unique_ptr<btMotionState> motionState;
	std::unique_ptr<btCollisionShape> collisionShape;

	std::unique_ptr<btTriangleMesh> triangleMesh;
	std::unique_ptr<btConvexHullShape> convexHullShape;

	int collisionGroup = -1;
	int collisionMask = -1;
};

}
}
