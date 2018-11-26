#include "SceneLoaderAdapter.hpp"

#include <Logger.hpp>
#include <Resources.hpp>

#include <components/RigidBodyComponent.hpp>
#include <components/TransformComponent.hpp>

#include <LinearMath/btDefaultMotionState.h>
#include <BulletCollision/CollisionShapes/btConvexHullShape.h>
#include <BulletCollision/CollisionShapes/btTriangleMesh.h>
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <components/ParentComponent.hpp>

#include <systems/render-engine/lights/PointLight.hpp>
#include <factories/LightFactory.hpp>

using namespace NAISE::Engine;

bool SceneLoaderAdapter::adapt(shared_ptr<Entity> entity, shared_ptr<Entity> parent, const tinygltf::Node& node,
								  const tinygltf::Model& model) const {
	if (node.name.compare(0, LIGHT_ID.size(), LIGHT_ID) == 0) {
		return handleLight(entity, node, model);
	}

	if (node.name.compare(0, TRIGGER_ID.size(), TRIGGER_ID) == 0) {
		return handleTrigger(entity, parent, node, model);
	}

	return false;
}

bool SceneLoaderAdapter::handleLight(shared_ptr<Entity> entity, const tinygltf::Node& node,
									 const tinygltf::Model& model) const {
	entity->add(LightFactory::createLight<PointLight>(vec3(0, 0, 0), vec3(50, 50, 50)));

	return false;
}


bool SceneLoaderAdapter::handleTrigger(shared_ptr<Entity> entity, shared_ptr<Entity> parent,
										  const tinygltf::Node& node, const tinygltf::Model& model) const {
	if (node.mesh < 0) {
		NAISE_WARN_CONSOL("Trigger volume contains no mesh!")
		return false;
	}

	NAISE_WARN_CONSOL("Trigger handling not implemented!")

	return false;
}

bool SceneLoaderAdapter::handleCamera(shared_ptr<Entity> entity, const tinygltf::Node& node,
									  const tinygltf::Model& model) const {
//	if (!entity->has<CameraComponent>()) {
//		NAISE_WARN_CONSOL("Camera is not a camera!")
//		return false;
//	}

	return false;
}

