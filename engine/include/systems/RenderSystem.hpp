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

namespace NAISE {
namespace Engine {

struct SunSignature: public Signature<LightComponent> {};
struct LightSignature: public Signature<LightComponent, TransformComponent> {};
struct GeometrySignature: public Signature<TransformComponent, MeshComponent> {};
struct VisualSignature: public Signature<TransformComponent, VisualComponent> {};
struct CameraSignature: public Signature<CameraComponent, TransformComponent> {};
struct DebugDrawSignature: public Signature<PhysicsDebugComponent> {};
struct ParticleRenderSignature: public Signature<TransformComponent, MeshParticleComponent> {};

using InstanceID = std::pair<Mesh*, Material*>;

class RenderSystem : public System {
public:

	RenderSystem();

	explicit RenderSystem(std::shared_ptr<RenderEngine> renderEngine);

	void process(microseconds deltaTime) override;

	void setSkybox(Skybox& skybox){
		this->skybox = skybox;
	}

private:
	bool cullEntity(Entity& camera, Entity& entity);

	std::shared_ptr<RenderEngine> _renderEngine;

	Skybox skybox = Skybox(glm::vec3(0.3,0.3,0.3));

	map<Mesh*, vector<glm::mat4>> shadowMeshInstances;
	map<InstanceID, vector<glm::mat4>> meshInstances;
};

}
}
