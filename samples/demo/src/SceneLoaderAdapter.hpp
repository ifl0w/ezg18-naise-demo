#pragma once

#include <resource-loader/ModelLoaderAdapter.hpp>
#include <components/RigidBodyComponent.hpp>
#include <components/TransformComponent.hpp>
#include <Resources.hpp>

using namespace NAISE::Engine;

class SceneLoaderAdapter: public ModelLoaderAdapter {
public:
	bool adapt(shared_ptr<Entity> entity, shared_ptr<Entity> parent, const tinygltf::Node& node,
			   const tinygltf::Model& model) const override;

private:
	std::string LIGHT_ID = "LIGHT::";
	std::string CAMERA_ID = "CAMERA::";
	std::string LIGHT_STRENGTH_ID = "LIGHT_STRENGTH::";
	std::string LIGHT_COLOR_ID = "LIGHT_COLOR::";
	std::string TRIGGER_ID = "TRIGGER::";
	std::string TRIGGER_ACTION_ID = "ACTION";

	bool handleLight(shared_ptr<Entity> entity, const tinygltf::Node& node,
							 const tinygltf::Model& model) const;

	bool handleCamera(shared_ptr<Entity> entity, const tinygltf::Node& node,
					 const tinygltf::Model& model) const;

	bool handleTrigger(shared_ptr<Entity> entity, shared_ptr<Entity> parent, const tinygltf::Node& node,
							 const tinygltf::Model& model) const;

	std::shared_ptr<RigidBodyComponent> staticMeshCollisionComponent(Mesh& mesh, TransformComponent& tc, bool convexHull = false) const;
};