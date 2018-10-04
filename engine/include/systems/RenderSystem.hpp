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
		geometryFilter.requirement<MaterialComponent>();

		lightFilter.requirement<TransformComponent>();
		lightFilter.requirement<LightComponent>();
	}

	void process(const EntityManager& em, microseconds deltaTime) override {
		Entity* camera = nullptr;
		Entity* sun = nullptr;

		em.filter(cameraFilter, [&](Entity* entity) { camera = entity; });
		em.filter(sunFilter, [&](Entity* entity) {
			if (entity->component<LightComponent>().light->data.directional) {
				sun = entity;
				return;
			}
		});

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
		  renderEngine.geometryPass(*entity.component<MeshComponent>().mesh.get(),
									*entity.component<MaterialComponent>().material.get(),
									entity.component<TransformComponent>().calculateModelMatrix());
		});
		renderEngine.deactivateRenderState();

		renderEngine.prepareLightPass();
		em.filter(lightFilter, [=](Entity& entity) {
		  auto& light = *entity.component<LightComponent>().light.get();
		  auto transComp = entity.component<TransformComponent>();

			if (entity.component<LightComponent>().isType<PointLight>()) {
				auto& l = dynamic_cast<PointLight&>(light);
				light.data.lightPosition = vec4(transComp.position, 1);
				light.data.lightPosition = vec4(transComp.position, 1);
				vec3 scale = vec3(l.calculateLightVolumeRadius());
				transComp.scale = scale;
			}

		  auto transform = transComp.calculateModelMatrix();

		  renderEngine.renderLights(light, transform, *camera);
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
