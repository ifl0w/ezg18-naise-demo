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

	void process(const EntityManager& em, microseconds deltaTime) override {
		Entity* camera = nullptr;
		Entity* sun = nullptr;

		renderEngine.setSkybox(&skybox);

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
			em.filter(geometryFilter, [=](vector<Entity*> entities) {
			  renderEngine.shadowPass(*sun, *camera, entities);
			});
		}

		renderEngine.initFrame(camera->component<CameraComponent>(), camera->component<TransformComponent>());

//		renderEngine.wireframe = true;
//		renderEngine.backfaceCulling = false;
		renderEngine.activateRenderState();
		em.filter(geometryFilter, [=](Entity& entity) {

		  Material* material = nullptr;
		  if (entity.has<MaterialComponent>()) {
			  material = entity.component<MaterialComponent>().material.get();
		  }
		  renderEngine.geometryPass(*entity.component<MeshComponent>().mesh.get(),
									material,
									entity.component<TransformComponent>().getModelMatrix());
		});
		renderEngine.deactivateRenderState();

		renderEngine.prepareLightPass();
		em.filter(lightFilter, [=](Entity& entity) {
		  auto& light = *entity.component<LightComponent>().light.get();
		  auto transComp = entity.component<TransformComponent>();
		  auto transform = transComp.getModelMatrix();

		  if (entity.component<LightComponent>().isType<PointLight>()) {
			  auto& l = dynamic_cast<PointLight&>(light);
			  light.data.lightPosition = vec4(glm::vec3(transform[3]), 1); // write back position
			  vec3 scale = vec3(l.calculateLightVolumeRadius());
			  transform = glm::scale(transform, scale);
		  }

		  renderEngine.renderLights(light, transform, *camera);
		});
		renderEngine.cleanupLightPass();

		//SKYBOX
		renderEngine.skyboxPass();

		//GLOW
		renderEngine.glowPass();

//		renderEngine.activateRenderState();
		em.filter(debugDrawFilter, [&](Entity& entity) {
		  auto& p = entity.component<PhysicsDebugComponent>();
		  renderEngine.drawDebugMesh(p.mesh, p.color);
		});
//		renderEngine.deactivateRenderState();
	};

	void setSkybox(Skybox& skybox){
		this->skybox = skybox;
	}

private:

	void processEntity(Entity& entity);

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
