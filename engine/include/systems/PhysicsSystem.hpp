#pragma once

#include "System.hpp"

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <LinearMath/btDefaultMotionState.h>
#include <glm/glm.hpp>

#include "physics/BulletDebugDrawer.hpp"

#define PHYSICS_SUBSTEPS 4

namespace NAISE {
namespace Engine {

class PhysicsSystem: public System {
public:
	PhysicsSystem();
	~PhysicsSystem();

	void process(const EntityManager& em, microseconds deltaTime) override;
private:
	Filter rigidBodyFilter;

	/* Physic properties */
	std::unique_ptr<btDiscreteDynamicsWorld> dynamicsWorld;
	std::unique_ptr<btBroadphaseInterface> broadphase;
	std::unique_ptr<BulletDebugDrawer> debugDrawer;
	std::unique_ptr<btDefaultCollisionConfiguration> collisionConfiguration;
	std::unique_ptr<btCollisionDispatcher> dispatcher;
	std::unique_ptr<btSequentialImpulseConstraintSolver> solver;

	// non owning list of entities
	std::vector<Entity*> rigidBodyEntities;

	bool physicsDebugging = false;
};

/**
 * Perform the physics update and calls this method for all children.
 * Locking dynamics world not needed since bullet does not work with the world during this callback.
 *
 * @param deltaTime
 */
void physicsTickCallback(btDynamicsWorld* world, double timeStep);

glm::vec3 btVector3ToVec3(btVector3 vec);
glm::quat btQuaternion3ToQuat(btQuaternion quat);

}
}
