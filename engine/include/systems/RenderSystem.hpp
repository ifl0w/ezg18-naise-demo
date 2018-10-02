#pragma once

#include "System.hpp"
#include "render-engine/RenderEngine.hpp"

#include <filter/Filter.hpp>
#include <scene/EntityManager.hpp>
#include <components/MeshComponent.hpp>
#include <components/TransformComponent.hpp>

#include <functional>

namespace NAISE {
namespace Engine {

class RenderSystem: public System {
public:

	RenderSystem() {
		cameraFilter.requirement<CameraComponent>();
		cameraFilter.requirement<TransformComponent>();

		geometryFilter.requirement<MeshComponent>();
		geometryFilter.requirement<TransformComponent>();
		geometryFilter.requirement<PhongMaterialComponent>();

		lightFilter.requirement<TransformComponent>();
		lightFilter.requirement<LightComponent>();
	}

	void process(const EntityManager& em) override {
		em.filter(cameraFilter, [=](Entity& entity) {
		  renderEngine.initFrame(entity.component<CameraComponent>());
		});

//		renderEngine.wireframe = true;
//		renderEngine.backfaceCulling = false;
		renderEngine.activateRenderState();
		em.filter(geometryFilter, [=](Entity& entity){
		  renderEngine.geometryPass(entity.component<MeshComponent>(),
									entity.component<TransformComponent>(),
									entity.component<PhongMaterialComponent>());
		});
		renderEngine.deactivateRenderState();

		renderEngine.prepareLightPass();
		em.filter(lightFilter, [=](Entity& entity) {
		  renderEngine.lightPass(entity.component<LightComponent>());
		});
		renderEngine.cleanupLightPass();
	};

private:

	void processEntity(Entity& entity);

	Filter cameraFilter;
	Filter geometryFilter;
	Filter lightFilter;

	RenderEngine renderEngine;
};

}
}
