#pragma once

#include "System.hpp"
#include "render-engine/RenderEngine.hpp"

#include <filter/Filter.hpp>
#include <scene/EntityManager.hpp>

#include <components/MeshComponent.hpp>
#include <components/TransformComponent.hpp>
#include <components/MaterialComponent.hpp>
#include <components/LightComponent.hpp>

#include <systems/render-engine/lights/PointLight.hpp>

#include <functional>
#include <systems/physics/BulletDebugDrawer.hpp>

namespace NAISE {
namespace Engine {

class RenderSystem : public System {
public:

	RenderSystem() {
		sunFilter.requirement<LightComponent>();

		shadowFilter.requirement<TransformComponent>();
		shadowFilter.requirement<MeshComponent>();

		cameraFilter.requirement<CameraComponent>();
		cameraFilter.requirement<TransformComponent>();

		geometryFilter.requirement<MeshComponent>();
		geometryFilter.requirement<TransformComponent>();

		lightFilter.requirement<TransformComponent>();
		lightFilter.requirement<LightComponent>();

		debugDrawFilter.requirement<PhysicsDebugComponent>();
	}

	void process(const EntityManager& em, microseconds deltaTime) override;

	void setSkybox(Skybox& skybox){
		this->skybox = skybox;
	}

private:

	bool cullEntity(Entity& camera, Entity& entity);

	Filter sunFilter;
	Filter shadowFilter;
	Filter cameraFilter;
	Filter geometryFilter;
	Filter lightFilter;
	Filter debugDrawFilter;

	RenderEngine renderEngine;

	Skybox skybox = Skybox(glm::vec3(0.3,0.3,0.3));
};

}
}
