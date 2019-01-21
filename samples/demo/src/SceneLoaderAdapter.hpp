#pragma once

#include <resource-loader/ModelLoaderAdapter.hpp>
#include <components/RigidBodyComponent.hpp>
#include <components/TransformComponent.hpp>
#include <Resources.hpp>

using namespace NAISE::Engine;

using CameraController = std::pair<EntityID, EntityID>;
using CameraSequence = std::map<int, CameraController>;

class SceneLoaderAdapter: public ModelLoaderAdapter {
public:
	bool adapt(shared_ptr<Entity> entity, shared_ptr<Entity> parent, const tinygltf::Node& node,
			   const tinygltf::Model& model) override;

	CameraSequence cameraSequence;
private:
	std::string LIGHT_ID = "LIGHT::";
	std::string CAMERA_ID = "CAMERA::";
	std::string LIGHT_STRENGTH_ID = "LIGHT_STRENGTH::";
	std::string LIGHT_COLOR_ID = "LIGHT_COLOR::";
	std::string TRIGGER_ID = "TRIGGER::";
	std::string TRIGGER_ACTION_ID = "ACTION";

	std::string INSTANCE_ID = "INSTANCE::";

	void handleInstance(shared_ptr<Entity>& entity, const tinygltf::Node& node) const;

	bool handleLight(shared_ptr<Entity> entity, const tinygltf::Node& node,
							 const tinygltf::Model& model) const;

	bool handleCamera(shared_ptr<Entity> entity, const tinygltf::Node& node,
					 const tinygltf::Model& model);

	bool handleTrigger(shared_ptr<Entity> entity, shared_ptr<Entity> parent, const tinygltf::Node& node,
							 const tinygltf::Model& model) const;


};