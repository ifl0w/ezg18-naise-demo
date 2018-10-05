#include <Engine.hpp>
#include <components/TransformComponent.hpp>
#include <components/MeshComponent.hpp>
#include <components/CameraComponent.hpp>
#include <components/MaterialComponent.hpp>
#include <components/InputComponent.hpp>
#include <components/LightComponent.hpp>

#include <systems/render-engine/materials/PBRMaterial.hpp>

#include <systems/render-engine/lights/Light.hpp>
#include <systems/render-engine/lights/DirectionalLight.hpp>
#include <systems/render-engine/lights/PointLight.hpp>

#include <factories/MeshFactory.hpp>
#include <factories/LightFactory.hpp>
#include <factories/MaterialFactory.hpp>

#include <Resources.hpp>

#include <Game.hpp>
#include <MovementSystem.hpp>

using namespace NAISE::Engine;

int main(int argc, char** argv) {
	NAISE::Engine::Engine engine;

	auto sphere = make_shared<NAISE::Engine::Entity>();
	sphere->add<TransformComponent>();
	sphere->component<TransformComponent>().position = vec3(-2, 0, -5);
	sphere->add(MeshFactory::createSphere());
	sphere->add<MaterialComponent>();
	sphere->add(MaterialFactory::createMaterial<PBRMaterial>(vec3(0.8, 0, 0.8), 1, 0.2));

	auto box = make_shared<NAISE::Engine::Entity>();
	box->add<TransformComponent>();
	box->component<TransformComponent>().position = vec3(0, -2, -5);
	box->component<TransformComponent>().scale = vec3(1, 1, 1);
	box->add(MeshFactory::createBox(20, 1, 20));
	box->add(MaterialFactory::createMaterial<PBRMaterial>(vec3(0.8, 0.8, 0.8), 0, 0.2));

	auto spaceship = Resources::loadModel("assets/models/spaceship/spaceship.gltf");
	spaceship->component<TransformComponent>().position = vec3(2, 0, -5);
	spaceship->component<TransformComponent>().scale = vec3(1, 1, 1);

	auto camera = make_shared<NAISE::Engine::Entity>();
	camera->add<TransformComponent>();
	camera->add<CameraComponent>();
	camera->add<InputComponent>();
	camera->component<InputComponent>().add<Actions::MoveForward>();
	camera->component<InputComponent>().add<Actions::MoveBackward>();
	camera->component<InputComponent>().add<Actions::MoveLeft>();
	camera->component<InputComponent>().add<Actions::MoveRight>();
	camera->component<InputComponent>().add<Actions::MouseMotion>();

	auto sun = make_shared<NAISE::Engine::Entity>();
	sun->add<TransformComponent>();
	sun->add(LightFactory::createLight<DirectionalLight>());
	sun->component<LightComponent>().light->data.direction = vec4(-1, -1, -1, 1);

	auto pointLight = make_shared<NAISE::Engine::Entity>();
	pointLight->add<TransformComponent>();
	pointLight->component<TransformComponent>().position = vec3(-2, -1, -7);
	pointLight->add(LightFactory::createLight<PointLight>());

	engine.entityManager.addEntity(sun);
	engine.entityManager.addEntity(pointLight);
	engine.entityManager.addEntity(camera);
	engine.entityManager.addEntity(sphere);
	engine.entityManager.addEntity(box);
	engine.entityManager.addEntity(spaceship);

	engine.inputSystem->setInputMapper(make_shared<GameInputMapper>());

	engine.addSystem(make_shared<MovementSystem>());

//	engine.mainWindow->setResolution(1920, 1200);
//	engine.mainWindow->captureMouse(true);
//	engine.mainWindow->setFullscreen(false);
	engine.run();
}