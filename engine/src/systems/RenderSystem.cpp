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

	auto& geometryEntities = Engine::getEntityManager().getEntities<GeometrySignature>();
	for (auto& instanceID: meshInstances) { instanceID.second.clear(); } // clear instances
	for (auto& instanceID: shadowMeshInstances) { instanceID.second.clear(); } // clear instances
	for (auto entity: geometryEntities) {
		Mesh* mesh = entity->component<MeshComponent>().mesh.get();

		// TODO: cull shadow meshes
		if(auto lightComp = sun->get<LightComponent>()) {
			// TODO fix shadow frustum culling
//			if (entity->has<AABBComponent>()) {
//				auto& entityAABB = entity->component<AABBComponent>().aabb;
//				auto* camComp = camera->get<CameraComponent>();
//				auto aabb = AABB(camComp->frustum.getBoundingVolume(20));
//				auto f = Frustum(aabb, glm::inverse(lightComp->light->getShadowMatrix()), 500);
//
//				if (f.intersect(entityAABB)) {
//					shadowMeshInstances[mesh].push_back(entity->component<TransformComponent>().getModelMatrix());
//				}
//			}

		}

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
	for (auto entity: Engine::getEntityManager().getEntities<VisualSignature>()) {
		auto meshes = entity->component<VisualComponent>().meshes;
		auto materials = entity->component<VisualComponent>().materials;
		for (int i = 0; i < meshes.size(); i++) {
			Mesh* mesh = meshes[i].get();

			// TODO: cull shadow meshes
			if(auto lightComp = sun->get<LightComponent>()) {
				// TODO fix shadow frustum culling
//				if (entity->has<AABBComponent>()) {
//					auto& entityAABB = entity->component<AABBComponent>().aabb;
//					auto* camComp = camera->get<CameraComponent>();
//					auto aabb = AABB(camComp->frustum.getBoundingVolume(20));
//					auto f = Frustum(aabb, glm::inverse(lightComp->light->getShadowMatrix()), 500);
//
//					if (f.intersect(entityAABB)) {
//						shadowMeshInstances[mesh].push_back(entity->component<TransformComponent>().getModelMatrix());
//					}
//				}

			}

			if (cullEntity(*camera, *entity)) {
				continue;
			}

			Material* material = materials[i].get();

			InstanceID instanceID(mesh, material);

			meshInstances[instanceID].push_back(entity->component<TransformComponent>().getModelMatrix());
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
		_renderEngine->activateShadowPass(*sun, *camera);
		for (auto& instance: shadowMeshInstances) {
			_renderEngine->drawMeshInstancedDirect(*instance.first, instance.second);
		}

//		_renderEngine->executeCommandBuffer(particleSystemCommandBuffer);

		_renderEngine->deactivateShadowPass();
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
			_renderEngine->renderLights(light, transform, *camera);
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
		auto aabb = AABB(c.frustum.getBoundingVolume(20));
		auto f = Frustum(aabb, (lc.light->getShadowMatrix()), 500);


		_renderEngine->drawDebugMesh(Mesh(aabb.obb), vec3(0,1,1));
		_renderEngine->drawDebugMesh(*f.frustumMesh.get(), vec3(1,0,1));
		_renderEngine->drawDebugMesh(*c.frustum.frustumMesh.get(), vec3(1,1,0));
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
