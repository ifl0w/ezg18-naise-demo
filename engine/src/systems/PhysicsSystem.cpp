#include <systems/PhysicsSystem.hpp>

#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <components/RigidBodyComponent.hpp>
#include <components/TransformComponent.hpp>

#include <Engine.hpp>

using namespace NAISE::Engine;

PhysicsSystem::PhysicsSystem() {
	Engine::getEntityManager().addSignature<RigidBodySignature>();
	Engine::getEntityManager().addSignature<CollisionSignature>();

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
	dynamicsWorld->setInternalTickCallback(physicsTickCallback, dynamicsWorld.get(), true);
	dynamicsWorld->setWorldUserInfo(this);

	/* Physics debug drawing */
	debugDrawer = std::make_unique<BulletDebugDrawer>();
	dynamicsWorld->setDebugDrawer(debugDrawer.get());
	debugDrawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawConstraints);

	Engine::getEventManager().event<RuntimeEvents::EntityAdded>().subscribe([&](EntityID id) {
	  Entity* newEntity = Engine::getEntityManager().getEntity(id);
	  if (!newEntity) { return; }

	  if (auto* rigidBodyComponent = newEntity->get<RigidBodyComponent>()) {
		  rigidBodyEntities.push_back(newEntity);
		  auto rigidBody = rigidBodyComponent->rigidBody.get();
		  rigidBody->setUserIndex(id);
		  rigidBody->activate(true);
		  dynamicsWorld->addRigidBody(rigidBody, rigidBodyComponent->collisionGroup, rigidBodyComponent->collisionMask);
	  }
	});

	Engine::getEventManager().event<RuntimeEvents::EntityRemoved>().subscribe([&](EntityID id) {
	  Entity* removeEntity = Engine::getEntityManager().getEntity(id);
	  if (!removeEntity) { return; }

	  if (auto* rigidBodyComponent = removeEntity->get<RigidBodyComponent>()) {
		  auto rigidBody = rigidBodyComponent->rigidBody.get();
		  dynamicsWorld->removeRigidBody(rigidBody);

		  auto it = find(rigidBodyEntities.begin(), rigidBodyEntities.end(), removeEntity);
		  rigidBodyEntities.erase(it);
	  }
	});

	// TODO: handle component removed and entity removed events.
}

void PhysicsSystem::process(microseconds deltaTime) {
	std::chrono::duration<float> sec = deltaTime;
	dynamicsWorld->stepSimulation(sec.count(), PHYSICS_SUBSTEPS, fixedTimestep);

	// TODO: check if the following code should be moved to the sub step callback
	auto& rgd = Engine::getEntityManager().getEntities<RigidBodySignature>();
	for (auto& entity: rgd) {
		auto rigidBody = entity->component<RigidBodyComponent>().rigidBody.get();

		if (!rigidBody->isStaticOrKinematicObject()) {
			auto& transform = entity->component<TransformComponent>();
			auto rigidTransform = rigidBody->getWorldTransform();

			transform.rotation = btQuaternion3ToQuat(rigidTransform.getRotation());
			transform.position = btVector3ToVec3(rigidTransform.getOrigin());
		} else {
			// TODO: this branch should probably be executed before the simulation. Investigate!
			auto& transform = entity->component<TransformComponent>();

			btTransform newWorldTransform;
			newWorldTransform.setRotation(btQuaternion(transform.rotation.x, transform.rotation.y, transform.rotation.z,
													   transform.rotation.w));
			newWorldTransform.setOrigin(btVector3(transform.position.x, transform.position.y, transform.position.z));

			if (rigidBody->isKinematicObject()) {
				rigidBody->getMotionState()->setWorldTransform(newWorldTransform);
			} else {
				rigidBody->setWorldTransform(newWorldTransform);
			}
		}
	}

	// clear collision components
	for (auto e: Engine::getEntityManager().getEntities<CollisionSignature>()) {
		e->component<CollisionComponent>().collisionEntities.clear();
		e->component<CollisionComponent>().collisionNormals.clear();
		e->component<CollisionComponent>().collisionPoints.clear();
	}
	// add collision data to collision components
	evaluateCollisions();
}

PhysicsSystem::~PhysicsSystem() {
	for (auto& subSys: subSystems) {
		subSys.second->resetSubSystem(this);
	}
	subSystems.clear();

	for (auto entity: rigidBodyEntities) {
		dynamicsWorld->removeRigidBody(entity->component<RigidBodyComponent>().rigidBody.get());
	}
	rigidBodyEntities.clear();
}

void PhysicsSystem::evaluateCollisions() {
	int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; i++) {
		btPersistentManifold* contactManifold = dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
		auto* obA = contactManifold->getBody0();
		Entity* enitiyA = Engine::getEntityManager().getEntity(static_cast<EntityID>(obA->getUserIndex()));
		auto* obB = contactManifold->getBody1();
		Entity* enitiyB = Engine::getEntityManager().getEntity(static_cast<EntityID>(obB->getUserIndex()));

		int numContacts = contactManifold->getNumContacts();
		for (int j = 0; j < numContacts; j++) {
			btManifoldPoint& pt = contactManifold->getContactPoint(j);
			if (pt.getDistance() < 0.f) {
				const btVector3& ptA = pt.getPositionWorldOnA();
				const btVector3& ptB = pt.getPositionWorldOnB();
				const btVector3& normalOnB = pt.m_normalWorldOnB;

				if (enitiyA != nullptr && enitiyB != nullptr) {
					if (auto* cc = enitiyA->get<CollisionComponent>()) {
						cc->collisionPoints.push_back(btVector3ToVec3(ptA));
						cc->collisionNormals.push_back(btVector3ToVec3(-normalOnB));
						cc->collisionEntities.push_back(enitiyB->id);
					}

					if (auto* cc = enitiyB->get<CollisionComponent>()) {
						cc->collisionPoints.push_back(btVector3ToVec3(ptB));
						cc->collisionNormals.push_back(btVector3ToVec3(normalOnB));
						cc->collisionEntities.push_back(enitiyA->id);
					}
				}
			}
		}
	}

}

void PhysicsSystem::toggleVisualDebugging() {
	this->physicsDebugging = !physicsDebugging;

	if (!physicsDebugging) {
		debugDrawer->disableDebugging();
	} else {
		debugDrawer->enableDebugging();
	}
}

void PhysicsSystem::processSubSystems(microseconds deltaTime) {
	if (physicsDebugging) {
		debugDrawer->beginDebugFrame();
	}

	for (const auto& type: systemsInsertionOrder) {
		subSystems[type]->processSubSystem(deltaTime); // process in correct order
	}

	if (physicsDebugging) {
		dynamicsWorld->debugDrawWorld();
		debugDrawer->finishMeshes();
	}
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

btVector3 NAISE::Engine::vec3ToBtVector3(vec3 vec) {
	return btVector3(vec.x, vec.y, vec.z);
}

void NAISE::Engine::physicsTickCallback(btDynamicsWorld* world, btScalar timeStep) {
	auto* physicsSystem = static_cast<PhysicsSystem*>(world->getWorldUserInfo());
	std::chrono::microseconds deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::duration<double>(timeStep));
	physicsSystem->processSubSystems(deltaTime);
}
