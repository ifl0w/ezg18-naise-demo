#include <Engine.hpp>
#include <components/TransformComponent.hpp>
#include <components/MeshComponent.hpp>
#include <components/CameraComponent.hpp>
#include <systems/render-engine/lights/LightComponent.hpp>
#include <systems/render-engine/materials/PhongMaterialComponent.hpp>
#include <meshes/MeshFactory.hpp>

#include "Game.hpp"

using namespace NAISE::Engine;

int main(int argc, char** argv) {
	NAISE::Engine::Engine engine;

	auto box = make_shared<NAISE::Engine::Entity>();
	box->add<TransformComponent>();
	box->component<TransformComponent>().position = vec3(0,0,-5);
	box->add(MeshFactory::createSphere());
	box->add<PhongMaterialComponent>();

	auto camera = make_shared<NAISE::Engine::Entity>();
	camera->add<TransformComponent>();
	camera->add<CameraComponent>();

	auto light = make_shared<NAISE::Engine::Entity>();
	light->add<TransformComponent>();
	light->add<LightComponent>();
	light->component<LightComponent>().data.directional = static_cast<uint32_t>(true);
	light->component<LightComponent>().data.direction = vec4(0,-1,-1,1);

	engine.entityManager.addEntity(light);
	engine.entityManager.addEntity(camera);
	engine.entityManager.addEntity(box);

	engine.run();
}