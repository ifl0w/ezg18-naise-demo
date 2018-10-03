#pragma once

#include "System.hpp"
#include "render-engine/RenderEngine.hpp"

#include <filter/Filter.hpp>
#include <scene/EntityManager.hpp>
#include <components/MeshComponent.hpp>
#include <components/TransformComponent.hpp>

#include <functional>
#include <systems/render-engine/lights/DirectionalLight.hpp>

namespace NAISE {
namespace Engine {

class RenderSystem : public System {
public:

	RenderSystem() {
		sunFilter.requirement<DirectionalLight>();

		shadowFilter.requirement<TransformComponent>();
		shadowFilter.requirement<MeshComponent>();

		cameraFilter.requirement<CameraComponent>();
		cameraFilter.requirement<TransformComponent>();

		geometryFilter.requirement<MeshComponent>();
		geometryFilter.requirement<TransformComponent>();
		geometryFilter.requirement<PhongMaterialComponent>();

		lightFilter.requirement<TransformComponent>();
		lightFilter.requirement<DirectionalLight>();
	}

	void process(const EntityManager& em, microseconds deltaTime) override {
		Entity* camera = nullptr;
		Entity* sun = nullptr;

		em.filter(cameraFilter, [&](Entity* entity) { camera = entity; });
		em.filter(sunFilter, [&](Entity* entity) { sun = entity; });

		if (camera == nullptr) {
			spdlog::get("logger")->warn("RenderSystem >> no active camera found.");
			return;
		}

		if (sun != nullptr) {
			em.filter(shadowFilter, [=](vector<Entity*> entities) {
			  renderEngine.shadowPass(*sun, *camera, entities);
			});
		}

		renderEngine.initFrame(camera->component<CameraComponent>(), camera->component<TransformComponent>());

//		renderEngine.wireframe = true;
//		renderEngine.backfaceCulling = false;
		renderEngine.activateRenderState();
		em.filter(geometryFilter, [=](Entity& entity) {
		  renderEngine.geometryPass(entity.component<MeshComponent>(),
									entity.component<TransformComponent>(),
									entity.component<PhongMaterialComponent>());
		});
		renderEngine.deactivateRenderState();

		renderEngine.prepareLightPass();
		em.filter(lightFilter, [=](Entity& entity) {
		  renderEngine.renderLights(entity.component<DirectionalLight>(), *camera);
		});
		renderEngine.cleanupLightPass();
	};

private:

	void processEntity(Entity& entity);

	Filter sunFilter;
	Filter shadowFilter;
	Filter cameraFilter;
	Filter geometryFilter;
	Filter lightFilter;

	RenderEngine renderEngine;
};

}
}
