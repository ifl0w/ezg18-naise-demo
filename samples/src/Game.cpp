#include <Engine.hpp>
#include <components/TransformComponent.hpp>
#include <components/MeshComponent.hpp>
#include <components/CameraComponent.hpp>
#include <systems/render-engine/lights/LightComponent.hpp>
#include <systems/render-engine/lights/DirectionalLight.hpp>
#include <systems/render-engine/materials/PhongMaterialComponent.hpp>
#include <meshes/MeshFactory.hpp>
#include <components/InputComponent.hpp>
#include <Game.hpp>
#include <MovementSystem.hpp>

using namespace NAISE::Engine;

int main(int argc, char** argv) {
	NAISE::Engine::Engine engine;

	auto sphere = make_shared<NAISE::Engine::Entity>();
	sphere->add<TransformComponent>();
	sphere->component<TransformComponent>().position = vec3(0, 0, -5);
	sphere->add(MeshFactory::createSphere());
	sphere->add<PhongMaterialComponent>(vec3(1,0,0));

	auto box = make_shared<NAISE::Engine::Entity>();
	box->add<TransformComponent>();
	box->component<TransformComponent>().position = vec3(0, -2, -5);
	box->component<TransformComponent>().scale = vec3(1, 1, 1);
	box->add(MeshFactory::createBox(20, 1, 20));
	box->add<PhongMaterialComponent>(vec3(1,0,1));

	auto camera = make_shared<NAISE::Engine::Entity>();
	camera->add<TransformComponent>();
	camera->add<CameraComponent>();
	camera->add<InputComponent>();
	camera->component<InputComponent>().add<Actions::MoveForward>();
	camera->component<InputComponent>().add<Actions::MoveBackward>();
	camera->component<InputComponent>().add<Actions::MoveLeft>();
	camera->component<InputComponent>().add<Actions::MoveRight>();
	camera->component<InputComponent>().add<Actions::MouseMotion>();

	auto light = make_shared<NAISE::Engine::Entity>();
	light->add<TransformComponent>();
	light->add<DirectionalLight>();
	light->component<DirectionalLight>();
	light->component<DirectionalLight>().data.direction = vec4(-1, -1, -1, 1);

	engine.entityManager.addEntity(light);
	engine.entityManager.addEntity(camera);
	engine.entityManager.addEntity(sphere);
	engine.entityManager.addEntity(box);

	engine.inputSystem->setInputMapper(make_shared<GameInputMapper>());

	engine.addSystem(make_shared<MovementSystem>());

//	engine.mainWindow->setResolution(1920, 1200);
//	engine.mainWindow->captureMouse(true);
//	engine.mainWindow->setFullscreen(false);
	engine.run();
}