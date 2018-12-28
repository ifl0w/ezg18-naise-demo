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

class PhysicsSystem;

struct PhysicsSubSystem {
  /**
   * Populate the dynamics world with rigid bodies ect.
   */
  virtual void initSubSystem(PhysicsSystem* physicsSystem) { };

  /**
   * Process the subsystem. This function will be called every internal substep of the bullet physics engine.
   * @param deltaTime
   */
  virtual void processSubSystem(microseconds deltaTime) = 0;

  /**
   * Cleanup the dynamics world
   */
  virtual void resetSubSystem(PhysicsSystem* physicsSystem) { };
};

class PhysicsSystem: public System {
public:
	PhysicsSystem();
	~PhysicsSystem();

	/* Physic properties */
	std::unique_ptr<btDiscreteDynamicsWorld> dynamicsWorld;

	void process(microseconds deltaTime) override;

	/**
	 * Display the bounding boxes and states of objects in the physics world.
	 */
	void toggleVisualDebugging();

	void processSubSystems(microseconds deltaTime);

	template<typename T, typename ... Args>
	void registerSubSystem(Args... args);

	template<typename T>
	T& getSubSystem();

	template<typename T>
	void removeSubSystem();

	const double simulationsPerSecond = 60.0;
	const double fixedTimestep = 1.0f / simulationsPerSecond; // timeStep < substeps * fixedTime

private:
	/* Physic properties */
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
	sys->initSubSystem(this);
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

	auto it = find(subSystems.begin(), subSystems.end(), type_index(typeid(T)));
	if (it != subSystems.end()) {
		it->second->resetSubSystem(this);
		subSystems.erase(it);

		systemsInsertionOrder.erase(remove_if(subSystems.begin(), subSystems.end(),
											  [](auto& s) { return s == type_index(typeid(T)); }));
	}
}

/**
 * Perform the physics update and calls this method for all children.
 * Locking dynamics world not needed since bullet does not work with the world during this callback.
 *
 * @param deltaTime
 */
void physicsTickCallback(btDynamicsWorld* world, btScalar timeStep);

glm::vec3 btVector3ToVec3(btVector3 vec);
btVector3 vec3ToBtVector3(glm::vec3 vec);
glm::quat btQuaternion3ToQuat(btQuaternion quat);

}
}
