#pragma once

#include "System.hpp"

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <LinearMath/btDefaultMotionState.h>
#include <glm/glm.hpp>

#include <components/RigidBodyComponent.hpp>
#include <components/TransformComponent.hpp>
#include <components/CollisionComponent.hpp>

#include "physics/BulletDebugDrawer.hpp"

#include "Logger.hpp"

#define PHYSICS_SUBSTEPS 4

namespace NAISE {
namespace Engine {

struct RigidBodySignature: public Signature<RigidBodyComponent, TransformComponent> {};
struct CollisionSignature: public Signature<RigidBodyComponent, CollisionComponent, TransformComponent> {};

struct PhysicsSubSystem {
  virtual void processSubSystem(double deltaTime) = 0;
};

class PhysicsSystem: public System {
public:
	PhysicsSystem();
	~PhysicsSystem();

	void process(microseconds deltaTime) override;

	/**
	 * Display the bounding boxes and states of objects in the physics world.
	 */
	void toggleVisualDebugging();

	void processSubSystems(double deltaTime);

	template<typename T, typename ... Args>
	void registerSubSystem(Args... args);

	template<typename T>
	T& getSubSystem();

	template<typename T>
	void removeSubSystem();

private:
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

	void evaluateCollisions();

	/* Sub system properties */
	std::unordered_map<std::type_index, std::shared_ptr<PhysicsSubSystem>> subSystems;
	std::vector<std::type_index> systemsInsertionOrder; // order is described by the position in the vector
};

template<typename T, typename... Args>
void PhysicsSystem::registerSubSystem(Args... args) {
	auto sys = std::make_shared<T>(args ...);
	auto type = std::type_index(typeid(T));
	subSystems.insert(pair(type, sys));
	systemsInsertionOrder.push_back(type);
}

template<typename T>
T& PhysicsSystem::getSubSystem() {
	auto idx = std::type_index(typeid(T));
	auto exits = subSystems.count(idx);

	if (!exits) {
		throw std::invalid_argument("SystemManager does not contain the requested system.");
	}

	auto tmp = static_cast<T*>(subSystems.at(idx).get());
	return *tmp;
}

template<typename T>
void PhysicsSystem::removeSubSystem() {
	NAISE_WARN_CONSOL("Remove system is untested! Functionality should be checked!");
	subSystems.erase(remove_if(subSystems.begin(), subSystems.end(),
							[](auto& s) { return typeid(s) == typeid(T); }));
	systemsInsertionOrder.erase(remove_if(subSystems.begin(), subSystems.end(),
										  [](auto& s) { return s == type_index(typeid(T)); }));
}

/**
 * Perform the physics update and calls this method for all children.
 * Locking dynamics world not needed since bullet does not work with the world during this callback.
 *
 * @param deltaTime
 */
void physicsTickCallback(btDynamicsWorld* world, btScalar timeStep);

glm::vec3 btVector3ToVec3(btVector3 vec);
glm::quat btQuaternion3ToQuat(btQuaternion quat);

}
}
