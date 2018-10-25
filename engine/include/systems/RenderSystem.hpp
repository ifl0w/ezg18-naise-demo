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

struct SunSignature: public Signature<LightComponent> {};
struct LightSignature: public Signature<LightComponent, TransformComponent> {};
struct GeometrySignature: public Signature<TransformComponent, MeshComponent> {};
struct CameraSignature: public Signature<CameraComponent, TransformComponent> {};
struct DebugDrawSignature: public Signature<PhysicsDebugComponent> {};

using InstanceID = std::pair<Mesh*, Material*>;

class RenderSystem : public System {
public:

	RenderSystem();

	void process(microseconds deltaTime) override;

	void setSkybox(Skybox& skybox){
		this->skybox = skybox;
	}

private:
	bool cullEntity(Entity& camera, Entity& entity);

	RenderEngine renderEngine;

	Skybox skybox = Skybox(glm::vec3(0.3,0.3,0.3));

	map<Mesh*, vector<glm::mat4>> shadowMeshInstances;
	map<InstanceID, vector<glm::mat4>> meshInstances;
};

}
}
