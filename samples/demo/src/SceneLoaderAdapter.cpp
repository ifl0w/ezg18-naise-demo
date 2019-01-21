#include "SceneLoaderAdapter.hpp"

#include <Logger.hpp>
#include <Resources.hpp>

#include <components/RigidBodyComponent.hpp>
#include <components/TransformComponent.hpp>
#include <components/CameraComponent.hpp>
#include <components/AnimationComponent.hpp>

#include <LinearMath/btDefaultMotionState.h>
#include <BulletCollision/CollisionShapes/btConvexHullShape.h>
#include <BulletCollision/CollisionShapes/btTriangleMesh.h>
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <components/ParentComponent.hpp>

#include <systems/render-engine/lights/PointLight.hpp>
#include <factories/LightFactory.hpp>
#include <resource-loader/GLTFLoader.hpp>
#include <Engine.hpp>

using namespace NAISE::Engine;

bool SceneLoaderAdapter::adapt(shared_ptr<Entity> entity, shared_ptr<Entity> parent, const tinygltf::Node& node,
								  const tinygltf::Model& model) {

	if (node.name.compare(0, INSTANCE_ID.size(), INSTANCE_ID) == 0) {
		handleInstance(entity, node);
	}

	if (node.name.compare(0, CAMERA_ID.size(), CAMERA_ID) == 0) {
		return handleCamera(entity, node, model);
	}

	if (node.name.compare(0, LIGHT_ID.size(), LIGHT_ID) == 0) {
		return handleLight(entity, node, model);
	}

	if (node.name.compare(0, TRIGGER_ID.size(), TRIGGER_ID) == 0) {
		return handleTrigger(entity, parent, node, model);
	}

	return false;
}


void SceneLoaderAdapter::handleInstance(shared_ptr<Entity>& entity, const tinygltf::Node& node) const {
	if (node.extras.IsObject() && node.extras.Has("path")) {
		auto sceneEntities = GLTFLoader::loadModel(node.extras.Get("path").Get<std::string>());
	} else {
		NAISE_WARN_CONSOL("INSTANCE tag without property 'path' will be ignored.")
		return;
	}

	auto sceneEntities = GLTFLoader::loadModel(node.extras.Get("path").Get<std::string>());
	for (auto& e: sceneEntities) {
		if (!e->has<ParentComponent>()) {
			e->add<ParentComponent>(entity->id);
		}
	}

	Engine::getEntityManager().addEntities(sceneEntities);
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
									  const tinygltf::Model& model) {

	std::string positionStr = node.name.substr(CAMERA_ID.size());
	int position = std::stoi(positionStr);
	auto camera = entity->get<CameraComponent>();
	auto animation = entity->get<TransformAnimationComponent>();

	if (camera != nullptr) {
		cameraSequence[position].first = entity->id;
	}

	if (animation != nullptr) {
		cameraSequence[position].second = entity->id;
	}

	return false;
}

