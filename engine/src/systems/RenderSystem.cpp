#include <systems/RenderSystem.hpp>
#include <components/AABBComponent.hpp>

#include <Logger.hpp>

using namespace NAISE::Engine;

void RenderSystem::process(const EntityManager& em, microseconds deltaTime) {
	Entity* camera = nullptr;
	Entity* sun = nullptr;

	// TODO: move somewhere to render engine
	// reset draw call count
	renderEngine.drawCallCount = 0;

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
	em.filter(geometryFilter, [&](Entity& entity) {
	  if (cullEntity(*camera, entity)) {
		  return;
	  }

	  Material* material = nullptr;
	  if (entity.has<MaterialComponent>()) {
		  material = entity.component<MaterialComponent>().material.get();
	  }
	  renderEngine.geometryPass(*entity.component<MeshComponent>().mesh.get(),
								material,
								entity.component<TransformComponent>().getModelMatrix());
	});
	renderEngine.deactivateRenderState();

//	NAISE_DEBUG_CONSOL("Draw calls: {}", renderEngine.drawCallCount)

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
}

bool RenderSystem::cullEntity(Entity& camera, Entity& entity) {
	if (camera.has<CameraComponent>() && entity.has<AABBComponent>()) {
		auto entityAABB = entity.component<AABBComponent>().aabb;
		auto& cameraFrustum = camera.component<CameraComponent>().frustum;

		return !cameraFrustum.intersect(entityAABB);
	}

	return false;
}
