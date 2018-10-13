#include <factories/RigidBodyFactory.hpp>
#include <LinearMath/btDefaultMotionState.h>
#include <BulletCollision/CollisionShapes/btSphereShape.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>

std::shared_ptr<NAISE::Engine::RigidBodyComponent> NAISE::Engine::RigidBodyFactory::createSphere(float radius,
																								 float mass,
																								 glm::vec3 position,
																								 bool kinematic) {
	auto rigidBody = std::make_shared<RigidBodyComponent>();

	rigidBody->motionState = std::make_unique<btDefaultMotionState>(
			btTransform(btQuaternion(0, 0, 0, 1), btVector3(position.x, position.y, position.z)));

	rigidBody->collisionShape = std::make_unique<btSphereShape>(radius);

	btVector3 fallInertia(0, 0, 0);
	rigidBody->collisionShape->calculateLocalInertia(mass, fallInertia);

	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, rigidBody->motionState.get(),
														 rigidBody->collisionShape.get(),
														 fallInertia);
	rigidBody->rigidBody = std::make_unique<btRigidBody>(rigidBodyCI);

	if (kinematic) {
		rigidBody->rigidBody->setCollisionFlags(rigidBody->rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		rigidBody->rigidBody->setActivationState(DISABLE_DEACTIVATION);
	}

	return rigidBody;
}

std::shared_ptr<NAISE::Engine::RigidBodyComponent> NAISE::Engine::RigidBodyFactory::createBox(
		float width, float height, float depth,
		float mass, glm::vec3 position,
		bool kinematic) {
	auto rigidBody = std::make_shared<RigidBodyComponent>();

	rigidBody->motionState = std::make_unique<btDefaultMotionState>(
			btTransform(btQuaternion(0, 0, 0, 1), btVector3(position.x, position.y, position.z)));

	rigidBody->collisionShape = std::make_unique<btBoxShape>(btVector3(width/2, height/2, depth/2));

	btVector3 fallInertia(0, 0, 0);
	rigidBody->collisionShape->calculateLocalInertia(mass, fallInertia);

	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, rigidBody->motionState.get(),
														 rigidBody->collisionShape.get(),
														 fallInertia);
	rigidBody->rigidBody = std::make_unique<btRigidBody>(rigidBodyCI);

	if (kinematic) {
		rigidBody->rigidBody->setCollisionFlags(rigidBody->rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		rigidBody->rigidBody->setActivationState(DISABLE_DEACTIVATION);
	}

	return rigidBody;
}
