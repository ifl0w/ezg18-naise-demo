#include <systems/RenderSystem.hpp>
#include <systems/WindowSystem.hpp>
#include <systems/particle-system/SimpleGPUParticleSystem.hpp>

#include <glm/gtx/quaternion.hpp>

#include <Engine.hpp>

using namespace NAISE::Engine;


RenderSystem::RenderSystem(): RenderSystem(std::make_shared<RenderEngine>()) { }

RenderSystem::RenderSystem(std::shared_ptr<RenderEngine> renderEngine): _renderEngine(renderEngine) {
	Engine::getEntityManager().addSignature<SunSignature>();
	Engine::getEntityManager().addSignature<LightSignature>();
	Engine::getEntityManager().addSignature<GeometrySignature>();
	Engine::getEntityManager().addSignature<CameraSignature>();
	Engine::getEntityManager().addSignature<DebugDrawSignature>();
	Engine::getEntityManager().addSignature<VisualSignature>();
	Engine::getEntityManager().addSignature<ParticleRenderSignature>();

	Engine::getEventManager().event<WindowEvents::SetResolution>().subscribe([&](uint32_t width, uint32_t height) {
	  _renderEngine->setResolution(width, height);
	});
}

void RenderSystem::process(microseconds deltaTime) {
	Entity* camera = nullptr;
	Entity* sun = nullptr;

	// TODO: move somewhere to render engine
	// reset draw call count
	_renderEngine->drawCallCount = 0;

	_renderEngine->setSkybox(&skybox);

	auto& cameraEntities = Engine::getEntityManager().getEntities<CameraSignature>();
	camera = cameraEntities.front();
	for (auto entity: cameraEntities) {
		if (entity->component<CameraComponent>().active) {
			camera = entity;
			break;
		}
	}

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

	auto lightComp = sun->get<LightComponent>();
	auto* camComp = camera->get<CameraComponent>();
	_cascadedShadowMapper->update(camComp->frustum, lightComp->light.get());

	auto& geometryEntities = Engine::getEntityManager().getEntities<GeometrySignature>();
	for (auto& instanceID: meshInstances) { instanceID.second.clear(); } // clear instances
	for (auto entity: geometryEntities) {
		Mesh* mesh = entity->component<MeshComponent>().mesh.get();
		auto aabb = entity->get<AABBComponent>();
		auto transform = entity->component<TransformComponent>().getModelMatrix();

		if(aabb != nullptr) {
			_cascadedShadowMapper->addShadowCaster(mesh, transform, aabb->aabb);
		}

		if (cullEntity(*camera, *entity)) {
			continue;
		}

		Material* material = nullptr;
		if (auto* matComp = entity->get<MaterialComponent>()) {
			material = matComp->material.get();
		}

		InstanceID instanceID(mesh, material);

		meshInstances[instanceID].push_back(transform);
	}
	for (auto entity: Engine::getEntityManager().getEntities<VisualSignature>()) {
		auto meshes = entity->component<VisualComponent>().meshes;
		auto materials = entity->component<VisualComponent>().materials;
		for (int i = 0; i < meshes.size(); i++) {
			Mesh* mesh = meshes[i].get();
			auto aabb = entity->get<AABBComponent>();
			auto transform = entity->component<TransformComponent>().getModelMatrix();

			if(aabb != nullptr) {
				_cascadedShadowMapper->addShadowCaster(mesh, transform, aabb->aabb);
			}

			if (cullEntity(*camera, *entity)) {
				continue;
			}

			Material* material = materials[i].get();

			InstanceID instanceID(mesh, material);

			meshInstances[instanceID].push_back(transform);
		}
	}

	RenderCommandBuffer particleSystemCommandBuffer;
	auto& particleSystemEntities = Engine::getEntityManager().getEntities<ParticleRenderSignature>();
	for (auto& particleSystem: particleSystemEntities) {
		auto& particleComponent = particleSystem->component<MeshParticleComponent>();
		auto& transformComponent = particleSystem->component<TransformComponent>();
		auto& mesh = particleComponent.mesh;
		auto& material = particleComponent.material;

		if (mesh && material) {
			DrawInstancedSSBO command;

			command.mesh = mesh.get();
			command.material = material.get();
			command.transformSSBO = particleComponent.ssboTransformations;
			command.originTransformation = transformComponent.getModelMatrix();
			command.count = particleComponent.particleCount;

			particleSystemCommandBuffer.push_back(command);
		}
	}

	if (sun != nullptr) {
		_renderEngine->executeCommandBuffer(_cascadedShadowMapper->generateCommandBuffer());
	}

	_renderEngine->initFrame(camera->component<CameraComponent>(), camera->component<TransformComponent>());

//		_renderEngine->wireframe = true;
//		_renderEngine->backfaceCulling = false;
	_renderEngine->activateRenderState();
	for (auto& instanceID: meshInstances) {
		if(!instanceID.second.empty()) {
			if(instanceID.second.size() > 1) {
				_renderEngine->drawMeshInstanced(*instanceID.first.first, instanceID.first.second, instanceID.second);
			} else {
				for (auto transform: instanceID.second) {
					_renderEngine->drawMesh(*instanceID.first.first, instanceID.first.second, transform);
				}
			}
		}
	}
	_renderEngine->executeCommandBuffer(particleSystemCommandBuffer);
	_renderEngine->deactivateRenderState();

//	NAISE_DEBUG_CONSOL("Draw calls: {}", _renderEngine->drawCallCount)

	auto& lightEntities = Engine::getEntityManager().getEntities<LightSignature>();
	_renderEngine->prepareLightPass();
	for (auto entity: lightEntities) {
		auto& light = *entity->component<LightComponent>().light.get();
		auto transComp = entity->component<TransformComponent>();
		auto transform = transComp.getModelMatrix();

		// TODO: move to light system
		if (entity->component<LightComponent>().isType<PointLight>()) {
			auto& l = dynamic_cast<PointLight&>(light);
			light.data.lightPosition = vec4(glm::vec3(transform[3]), 1); // write back position
			vec3 scale = vec3(l.calculateLightVolumeRadius());
			transform = glm::scale(transform, scale);
		}

		if (entity->component<LightComponent>().isType<SpotLight>()) {
			auto* l = dynamic_cast<SpotLight*>(&light);
			light.data.lightPosition = vec4(glm::vec3(transform[3]), 1); // write back position
			light.data.direction = glm::vec4(glm::rotate(transComp.globalRotation, vec3(0,0,-1)), 1) ; // default direction is (0, 0, -1)
			vec3 scale = vec3(l->calculateLightVolumeRadius());
			transform = glm::scale(transform, scale);
		}

		if (!cullEntity(*camera, *entity)) {
			_renderEngine
					->renderLights(light, transform, *camera, _cascadedShadowMapper->shadowCascades, _cascadedShadowMapper->cascades);
		}
	}
	_renderEngine->cleanupLightPass();

	//SKYBOX
	_renderEngine->skyboxPass();

	//GLOW
	_renderEngine->glowPass();

	// HDR
	std::chrono::duration<float> sec = deltaTime;
	_renderEngine->hdrPass(sec.count());

	auto& debugDrawEntities = Engine::getEntityManager().getEntities<DebugDrawSignature>();
//		_renderEngine->activateRenderState();
	for (auto& entity: debugDrawEntities) {
		auto& p = entity->component<PhysicsDebugComponent>();
		_renderEngine->drawDebugMesh(p.mesh, p.color);
	}

	for (auto& entity: cameraEntities) {
		auto& c = entity->component<CameraComponent>();
		if (c.active) {
			continue;
		}

		auto& lc = sun->component<LightComponent>();

		_cascadedShadowMapper->update(c.frustum, lc.light.get());
		_renderEngine->executeCommandBuffer(_cascadedShadowMapper->generateDebugCommandBuffer());
	}
//		_renderEngine->deactivateRenderState();

	_renderEngine->resolveFrameBufferObject();
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
