#include <utility>

#include <utility>

#pragma once

#include "System.hpp"
#include "render-engine/RenderEngine.hpp"

#include <filter/Filter.hpp>
#include <scene/EntityManager.hpp>

#include <components/MeshComponent.hpp>
#include <components/TransformComponent.hpp>
#include <components/MaterialComponent.hpp>
#include <components/LightComponent.hpp>
#include <components/VisualComponent.hpp>

#include <systems/render-engine/lights/PointLight.hpp>
#include <systems/render-engine/lights/SpotLight.hpp>

#include <functional>
#include <systems/physics/BulletDebugDrawer.hpp>
#include <systems/particle-system/MeshParticleComponent.hpp>

#include <systems/render-engine/shadow-map/CascadedShadowMapper.hpp>

namespace NAISE {
namespace Engine {

struct SunSignature : public Signature<LightComponent> { };
struct LightSignature : public Signature<LightComponent, TransformComponent> { };
struct GeometrySignature : public Signature<TransformComponent, MeshComponent> { };
struct VisualSignature : public Signature<TransformComponent, VisualComponent> { };
struct CameraSignature : public Signature<CameraComponent, TransformComponent> { };
struct DebugDrawSignature : public Signature<PhysicsDebugComponent> { };
struct ParticleRenderSignature : public Signature<TransformComponent, MeshParticleComponent> { };

using InstanceID = std::pair<Mesh*, Material*>;

class RenderSystem : public System {
public:

	RenderSystem();

	explicit RenderSystem(std::shared_ptr<RenderEngine> renderEngine);

	void process(microseconds deltaTime) override;

	void setSkybox(Skybox& skybox) {
		this->skybox = skybox;
	}

private:
	std::shared_ptr<RenderEngine> _renderEngine;
	std::unique_ptr<CascadedShadowMapper> _cascadedShadowMapper = std::make_unique<CascadedShadowMapper>();

	Skybox skybox = Skybox(glm::vec3(0.3, 0.3, 0.3));

	map<InstanceID, vector<glm::mat4>> meshInstances;

	/**
	 * Render properties
	 */
	Entity* _activeCamera = nullptr;
	Entity* _activeSun = nullptr;
	glm::mat4 previousViewMatrix = glm::mat4(1);
	glm::mat4 previousProjectionMatrix = glm::mat4(1);

	bool _visualDebugging = false;
	bool _drawWireframe = false;
	bool _drawCameraObjects = false;
	bool _drawPhysicsDebugObjects = false;
	bool _drawAABBs = false;

	/**
	 * Produces the commandbuffer for filling the gBuffer and the shadow maps.
	 * @return
	 */
	RenderCore::RenderCommandBuffer _gBufferRenderBuffer();
	uint64_t _commandBufferSize = 100;

	/**
	 * Produces the commandbuffer for rendering the particle systems.
	 * @return
	 */
	RenderCore::RenderCommandBuffer _meshParticlesRenderBuffer();
	uint64_t _particleCommandBufferSize = 100;

	/**
	 * Light rendering (light pass)
	 */
	RenderCore::RenderCommandBuffer _lightsCommandBuffer();
	uint64_t _lightCommandBufferSize = 10;

	/**
	 * Debug objects
	 */
	RenderCore::RenderCommandBuffer _debugCommandBuffer();

	void _postProcessing(std::chrono::microseconds deltaTime);

	/**
	 * Iterates over all renderable objects and tests if they are
	 * relevant for the current frame and inserts them in the
	 * _meshInstaces vector.
	 */
	void _collectGeometries();

	/**
	 * Cull an entity against the given camera frustum.
	 * Returns true if the object does not intersect/include with the camera frustum else false.
	 *
	 * @param camera
	 * @param entity
	 * @return bool
	 */
	bool _cullEntity(Entity& camera, Entity& entity);

};

}
}
