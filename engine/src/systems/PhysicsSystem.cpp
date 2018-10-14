#include <systems/PhysicsSystem.hpp>

#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <components/RigidBodyComponent.hpp>
#include <components/TransformComponent.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Engine.hpp>

using namespace NAISE::Engine;

PhysicsSystem::PhysicsSystem() {
	rigidBodyFilter.requirement<RigidBodyComponent>();
	rigidBodyFilter.requirement<TransformComponent>();

	// Build the broadphase
	broadphase = std::make_unique<btDbvtBroadphase>();

	// Set up the collision configuration and dispatcher
	collisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();
	dispatcher = std::make_unique<btCollisionDispatcher>(collisionConfiguration.get());

	// The actual physics solver
	solver = std::make_unique<btSequentialImpulseConstraintSolver>();

	/* The physics world. */
	dynamicsWorld = std::make_unique<btDiscreteDynamicsWorld>(dispatcher.get(), broadphase.get(), solver.get(),
															  collisionConfiguration.get());
	dynamicsWorld->setGravity(btVector3(0, -9.81, 0));
	dynamicsWorld->getPairCache()->setInternalGhostPairCallback(new btGhostPairCallback()); // enable ghost objects
//	dynamicsWorld->setInternalTickCallback(physicsTickCallback, nullptr, true);

	/* Physics debug drawing */
	debugDrawer = std::make_unique<BulletDebugDrawer>();
	dynamicsWorld->setDebugDrawer(debugDrawer.get());
	debugDrawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawConstraints);

	Engine::getEventManager().event<RuntimeEvents::EntityAdded>().subscribe([&](EntityID id) {
		Entity* newEntity = Engine::getEntityManager().getEntity(id);

		if (newEntity && newEntity->has<RigidBodyComponent>()) {
			rigidBodyEntities.push_back(newEntity);
			auto rigidBody = newEntity->component<RigidBodyComponent>().rigidBody.get();
			rigidBody->activate(true);
			dynamicsWorld->addRigidBody(rigidBody);
		}
	});
}

void PhysicsSystem::process(const EntityManager& em, microseconds deltaTime) {
	std::chrono::duration<float> sec = deltaTime;
	dynamicsWorld->stepSimulation(sec.count(), PHYSICS_SUBSTEPS, 1.0f/60.0f); // timeStep < substeps * fixedTime

	em.filter(rigidBodyFilter, [&](Entity& entity){
	  auto rigidBody = entity.component<RigidBodyComponent>().rigidBody.get();

	  if(!rigidBody->isKinematicObject()) {
		  auto& transform = entity.component<TransformComponent>();
		  auto rigidTransform = rigidBody->getWorldTransform();

		  transform.rotation = btQuaternion3ToQuat(rigidTransform.getRotation());
		  transform.position = btVector3ToVec3(rigidTransform.getOrigin());
	  } else {
		  auto& transform = entity.component<TransformComponent>();

		  btTransform newWorldTransform;
		  newWorldTransform.setFromOpenGLMatrix(glm::value_ptr(dmat4(transform.calculateModelMatrix())));

		  rigidBody->getMotionState()->setWorldTransform(newWorldTransform);
	  }
	});

	if (physicsDebugging) {
		debugDrawer->beginDebugFrame();
		dynamicsWorld->debugDrawWorld();
		debugDrawer->finishMeshes();
	}
}

PhysicsSystem::~PhysicsSystem() {
	for (auto entity: rigidBodyEntities) {
		dynamicsWorld->removeRigidBody(entity->component<RigidBodyComponent>().rigidBody.get());
	}

	rigidBodyEntities.clear();
}

glm::quat NAISE::Engine::btQuaternion3ToQuat(btQuaternion q) {
	btScalar x = 0;
	btScalar y = 0;
	btScalar z = 0;
	q.getEulerZYX(z, y, x);
	return glm::quat(vec3(x, y, z));
}

glm::vec3 NAISE::Engine::btVector3ToVec3(btVector3 vec) {
	return vec3(vec.getX(), vec.getY(), vec.getZ());
}

void NAISE::Engine::physicsTickCallback(btDynamicsWorld* world, double timeStep) {
	spdlog::get("console")->debug("PhysicsSystem >> Substep callback not implemented.");
}