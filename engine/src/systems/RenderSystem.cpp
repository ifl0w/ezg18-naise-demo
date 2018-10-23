#include <systems/RenderSystem.hpp>
#include <components/AABBComponent.hpp>

#include <Engine.hpp>

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

	auto& cameraEntities = Engine::getEntityManager().getEntities<CameraSignature>();
	for (auto entity: cameraEntities) { camera = entity; }

	auto& sunEntities = Engine::getEntityManager().getEntities<SunSignature>();
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

	auto& geometryEntities = Engine::getEntityManager().getEntities<GeometrySignature>();
	for (auto& instanceID: meshInstances) {	instanceID.second.clear(); } // clear instances
	for (auto& instanceID: shadowMeshInstances) {	instanceID.second.clear(); } // clear instances
	for (auto entity: geometryEntities) {
		Mesh* mesh = entity->component<MeshComponent>().mesh.get();

		// TODO: cull shadow meshes
		shadowMeshInstances[mesh].push_back(entity->component<TransformComponent>().getModelMatrix());

		if (cullEntity(*camera, *entity)) {
			continue;
		}

		Material* material = nullptr;
		if (auto* matComp = entity->get<MaterialComponent>()) {
			material = matComp->material.get();
		}

		InstanceID instanceID(mesh, material);

		meshInstances[instanceID].push_back(entity->component<TransformComponent>().getModelMatrix());
	}

	if (sun != nullptr) {
		renderEngine.activateShadowPass(*sun, *camera);
		for (auto& instance: shadowMeshInstances) {
			renderEngine.drawMeshInstancedDirect(*instance.first, instance.second);
		}
		renderEngine.deactivateShadowPass();
	}

	renderEngine.initFrame(camera->component<CameraComponent>(), camera->component<TransformComponent>());

//		renderEngine.wireframe = true;
//		renderEngine.backfaceCulling = false;
	renderEngine.activateRenderState();
	for (auto& instanceID: meshInstances) {
		renderEngine.drawMeshInstanced(*instanceID.first.first, instanceID.first.second, instanceID.second);
	}
	renderEngine.deactivateRenderState();

//	NAISE_DEBUG_CONSOL("Draw calls: {}", renderEngine.drawCallCount)

	auto& lightEntities = Engine::getEntityManager().getEntities<LightSignature>();
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

	auto& debugDrawEntities = Engine::getEntityManager().getEntities<DebugDrawSignature>();
//		renderEngine.activateRenderState();
	for (auto& entity: debugDrawEntities) {
		auto& p = entity->component<PhysicsDebugComponent>();
		renderEngine.drawDebugMesh(p.mesh, p.color);
	}
//		renderEngine.deactivateRenderState();
}

bool RenderSystem::cullEntity(Entity& camera, Entity& entity) {
	auto* camComp = camera.get<CameraComponent>();
	auto* aabbComp = entity.get<AABBComponent>();
	if (camComp && aabbComp) {
		auto entityAABB = aabbComp->aabb;
		auto& cameraFrustum = camComp->frustum;

		return !cameraFrustum.intersect(entityAABB);
	}

	return false;
}
