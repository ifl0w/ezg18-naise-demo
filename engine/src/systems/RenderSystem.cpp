#include <systems/RenderSystem.hpp>
#include <components/AABBComponent.hpp>

#include <Engine.hpp>
#include <Logger.hpp>

using namespace NAISE::Engine;

RenderSystem::RenderSystem() {
	Engine::getEntityManager().addSignature<SunSignature>();
	Engine::getEntityManager().addSignature<LightSignature>();
	Engine::getEntityManager().addSignature<GeometrySignature>();
	Engine::getEntityManager().addSignature<CameraSignature>();
	Engine::getEntityManager().addSignature<DebugDrawSignature>();
}


void RenderSystem::process(const EntityManager& em, microseconds deltaTime) {
	Entity* camera = nullptr;
	Entity* sun = nullptr;

	// TODO: move somewhere to render engine
	// reset draw call count
	renderEngine.drawCallCount = 0;

	renderEngine.setSkybox(&skybox);

	auto& cameraEntities = Engine::getEntityManager().getSignature<CameraSignature>()->entities;
	for (auto entity: cameraEntities) { camera = entity; }

	auto& sunEntities = Engine::getEntityManager().getSignature<SunSignature>()->entities;
	for (auto entity: sunEntities) {
		if (entity->component<LightComponent>().light->data.directional) {
			sun = entity;
			break;
		}
	}

	if (camera == nullptr) {
		NAISE_WARN_CONSOL("No active camera found.")
		return;
	}

	auto& geometryEntities = Engine::getEntityManager().getSignature<GeometrySignature>()->entities;
	if (sun != nullptr) {
		renderEngine.shadowPass(*sun, *camera, geometryEntities);
	}

	renderEngine.initFrame(camera->component<CameraComponent>(), camera->component<TransformComponent>());

//		renderEngine.wireframe = true;
//		renderEngine.backfaceCulling = false;
	renderEngine.activateRenderState();
	for (auto entity: geometryEntities) {
		if (cullEntity(*camera, *entity)) {
			continue;
		}

		Material* material = nullptr;
		if (entity->has<MaterialComponent>()) {
			material = entity->component<MaterialComponent>().material.get();
		}
		renderEngine.geometryPass(*entity->component<MeshComponent>().mesh.get(),
								  material,
								  entity->component<TransformComponent>().getModelMatrix());
	}
	renderEngine.deactivateRenderState();

//	NAISE_DEBUG_CONSOL("Draw calls: {}", renderEngine.drawCallCount)

	auto& lightEntities = Engine::getEntityManager().getSignature<LightSignature>()->entities;
	renderEngine.prepareLightPass();
	for (auto entity: lightEntities) {
		auto& light = *entity->component<LightComponent>().light.get();
		auto transComp = entity->component<TransformComponent>();
		auto transform = transComp.getModelMatrix();

		if (entity->component<LightComponent>().isType<PointLight>()) {
			auto& l = dynamic_cast<PointLight&>(light);
			light.data.lightPosition = vec4(glm::vec3(transform[3]), 1); // write back position
			vec3 scale = vec3(l.calculateLightVolumeRadius());
			transform = glm::scale(transform, scale);
		}

		renderEngine.renderLights(light, transform, *camera);
	}
	renderEngine.cleanupLightPass();

	//SKYBOX
	renderEngine.skyboxPass();

	//GLOW
	renderEngine.glowPass();

	auto& debugDrawEntities = Engine::getEntityManager().getSignature<DebugDrawSignature>()->entities;
//		renderEngine.activateRenderState();
	for (auto& entity: debugDrawEntities) {
		auto& p = entity->component<PhysicsDebugComponent>();
		renderEngine.drawDebugMesh(p.mesh, p.color);
	}
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
