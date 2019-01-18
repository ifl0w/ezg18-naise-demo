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
	_renderEngine->setSkybox(&skybox);

	auto& cameraEntities = Engine::getEntityManager().getEntities<CameraSignature>();
	_activeCamera = cameraEntities.front();
	for (auto entity: cameraEntities) {
		if (entity->component<CameraComponent>().active) {
			_activeCamera = entity;
			break;
		}
	}

	auto& sunEntities = Engine::getEntityManager().getEntities<SunSignature>();
	for (auto entity: sunEntities) {
		if (entity->component<LightComponent>().light->data.directional) {
			_activeSun = entity;
			break;
		}
	}

	if (_activeCamera == nullptr) {
		NAISE_WARN_CONSOL("No active camera found.")
		return;
	}

	if (_activeSun != nullptr) {
		auto lightComp = _activeSun->get<LightComponent>();
		auto* camComp = _activeCamera->get<CameraComponent>();
		_cascadedShadowMapper->update(camComp->frustum, lightComp->light.get());
	}

	_collectGeometries();

	_renderEngine->executeCommandBuffer(_cascadedShadowMapper->generateCommandBuffer());

	_renderEngine->executeCommandBuffer(_gBufferRenderBuffer());
	_renderEngine->executeCommandBuffer(_meshParticlesRenderBuffer());

	_renderEngine->executeCommandBuffer(_lightsCommandBuffer());

	_postProcessing(deltaTime);

//	NAISE_DEBUG_CONSOL("Draw calls: {}", _renderEngine->drawCallCount)
	_renderEngine->resolveFrameBufferObject();

	if (_visualDebugging) {
		_renderEngine->executeCommandBuffer(_debugCommandBuffer());
	}

	previousViewMatrix = glm::inverse(_activeCamera->component<TransformComponent>().getModelMatrix());
	previousProjectionMatrix = _activeCamera->component<CameraComponent>().getProjectionMatrix();
}

void RenderSystem::_postProcessing(std::chrono::microseconds deltaTime) {
	//SKYBOX
	_renderEngine->skyboxPass();

	//GLOW
	_renderEngine->glowPass();

	// HDR
	std::chrono::duration<float> sec = deltaTime;
	_renderEngine->hdrPass(sec.count());

	_renderEngine->motionBlurPass(sec.count(), previousViewMatrix, previousProjectionMatrix);
}

bool RenderSystem::_cullEntity(Entity& camera, Entity& entity) {
	auto* camComp = camera.get<CameraComponent>();
	auto* aabbComp = entity.get<AABBComponent>();
	if (camComp && aabbComp) {
		auto entityAABB = aabbComp->aabb;
		auto& cameraFrustum = camComp->frustum;

		return !cameraFrustum.intersect(entityAABB);
	}

	return false;
}

RenderCommandBuffer RenderSystem::_gBufferRenderBuffer() {
	RenderCommandBuffer buffer;
	buffer.reserve(_commandBufferSize);

	buffer.push_back(SetRenderTarget {_renderEngine->deferredTarget.get()});

	// enable back face culling
	buffer.push_back(SetRenderProperty { BACKFACE_CULLING, true });

	// enable depth test
	buffer.push_back(SetRenderProperty { DEPTH_TEST, true });

	buffer.push_back(ClearRenderTarget {true, true});

	buffer.push_back(SetViewProjectionData {
			glm::inverse(_activeCamera->component<TransformComponent>().getModelMatrix()),
			_activeCamera->component<CameraComponent>().getProjectionMatrix(),
			_activeCamera->component<TransformComponent>().globalPosition
	});

	if (_drawWireframe) {
		// TODO
		// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {

	}

	// append draw commands
	for (auto& instance: meshInstances) {
		if (instance.second.size() > 1) {
			buffer.push_back(DrawInstanced { instance.first.first, instance.first.second, instance.second });
		} else {
			for (auto transform: instance.second) {
				buffer.push_back(DrawMesh { instance.first.first, instance.first.second, transform });
			}
		}
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);

	_commandBufferSize = glm::max(_commandBufferSize, buffer.size());
	return buffer;
}


void RenderSystem::_collectGeometries() {

	auto& geometryEntities = Engine::getEntityManager().getEntities<GeometrySignature>();
	for (auto& instanceID: meshInstances) { instanceID.second.clear(); } // clear instances
	for (auto entity: geometryEntities) {
		Mesh* mesh = entity->component<MeshComponent>().mesh.get();
		auto aabb = entity->get<AABBComponent>();
		auto transform = entity->component<TransformComponent>().getModelMatrix();

		if(aabb != nullptr) {
			_cascadedShadowMapper->addShadowCaster(mesh, transform, aabb->aabb);
		}

		if (_cullEntity(*_activeCamera, *entity)) {
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

			if (_cullEntity(*_activeCamera, *entity)) {
				continue;
			}

			Material* material = materials[i].get();

			InstanceID instanceID(mesh, material);

			meshInstances[instanceID].push_back(transform);
		}
	}

}

RenderCommandBuffer RenderSystem::_meshParticlesRenderBuffer() {
	RenderCommandBuffer buffer;
	buffer.reserve(_particleCommandBufferSize);

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

			buffer.push_back(command);
		}
	}

	_particleCommandBufferSize = glm::max(_particleCommandBufferSize, buffer.size());
	return buffer;
}

RenderCommandBuffer RenderSystem::_lightsCommandBuffer() {
	RenderCommandBuffer buffer;
	buffer.reserve(_lightCommandBufferSize);

	// TODO: rework lighting. (Light culling in compute shader)

	buffer.push_back(SetRenderTarget {_renderEngine->lightTarget.get()});

	buffer.push_back(SetClearColor { vec4(0,0,0,0) });
	buffer.push_back(ClearRenderTarget {true, true});

	buffer.push_back(RetrieveDepthBuffer {_renderEngine->deferredTarget.get(), _renderEngine->lightTarget.get()});

	// enable depth test
	buffer.push_back(SetRenderProperty { DEPTH_TEST, true });
	// do not write to the depth buffer
	buffer.push_back(SetRenderProperty { DEPTH_MASK, false });

	// "add" light
	buffer.push_back(SetRenderProperty { BLEND, true });
	buffer.push_back(SetBlendMode { ADD });

	auto& lightEntities = Engine::getEntityManager().getEntities<LightSignature>();
	for (auto entity: lightEntities) {
		if (_cullEntity(*_activeCamera, *entity)) {
			continue;
		}

		auto& light = *entity->component<LightComponent>().light.get();
		auto transComp = entity->component<TransformComponent>();
		auto transform = transComp.getModelMatrix();

		// TODO: move to light system
		if (entity->component<LightComponent>().isType<PointLight>()) {
			auto& l = dynamic_cast<PointLight&>(light);

			light.data.lightPosition = vec4(glm::vec3(transform[3]), 1); // write back position
			vec3 scale = vec3(l.calculateLightVolumeRadius());

			buffer.push_back(RenderPointLight {
				entity->component<LightComponent>().light.get(),
				glm::scale(transform, scale)
			});

			continue;
		}

		if (entity->component<LightComponent>().isType<SpotLight>()) {
			auto* l = dynamic_cast<SpotLight*>(&light);

			light.data.lightPosition = vec4(glm::vec3(transform[3]), 1); // write back position
			light.data.direction = glm::vec4(glm::rotate(transComp.globalRotation, vec3(0,0,-1)), 1) ; // default direction is (0, 0, -1)
			vec3 scale = vec3(l->calculateLightVolumeRadius());

			buffer.push_back(RenderPointLight {
					entity->component<LightComponent>().light.get(),
					glm::scale(transform, scale)
			});

			continue;
		}

		buffer.push_back(RenderDirectionalLight {
				entity->component<LightComponent>().light.get(),
				_cascadedShadowMapper->cascades,
				_activeCamera->component<CameraComponent>().getProjectionMatrix()
		});
	}

	// write to the depth buffer again
	buffer.push_back(SetRenderProperty { DEPTH_MASK, true });

	buffer.push_back(SetRenderProperty { BLEND, false });

	_lightCommandBufferSize = glm::max(_lightCommandBufferSize, buffer.size());
	return buffer;
}

RenderCommandBuffer RenderSystem::_debugCommandBuffer() {
	RenderCommandBuffer buffer;

	buffer.push_back(RetrieveDepthBuffer {_renderEngine->deferredTarget.get()});

	auto& debugDrawEntities = Engine::getEntityManager().getEntities<DebugDrawSignature>();
	for (auto& entity: debugDrawEntities) {
		auto& p = entity->component<PhysicsDebugComponent>();

		buffer.push_back(DrawWireframeDirect{&p.mesh, mat4(1), p.color});
	}

	buffer.push_back(SetRenderProperty { DEPTH_TEST, false });

	auto& cameraEntities = Engine::getEntityManager().getEntities<CameraSignature>();
	for (auto& entity: cameraEntities) {
		auto& c = entity->component<CameraComponent>();
		if (c.active) {
			continue;
		}

		auto& lc = _activeSun->component<LightComponent>();

		_cascadedShadowMapper->update(c.frustum, lc.light.get());
		buffer.append(_cascadedShadowMapper->generateDebugCommandBuffer());
	}

	buffer.push_back(SetRenderProperty { DEPTH_TEST, true });

	return buffer;
}

