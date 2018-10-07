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


	auto camera = make_shared<NAISE::Engine::Entity>();
	camera->add<TransformComponent>();
	camera->component<TransformComponent>().position = vec3(0, 1.6, 0);
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
	sun->component<LightComponent>().light->data.direction = vec4(-1, -2, -1, 1);
	sun->component<LightComponent>().light->data.ambient = vec4(0.75, 0.75, 0.75, 1);

	for (int i = 0; i < 5; ++i) {
		auto pointLight = make_shared<NAISE::Engine::Entity>();
		pointLight->add<TransformComponent>();
		pointLight->component<TransformComponent>().position = vec3(0, 0, -i * 20);
		pointLight->add(LightFactory::createLight<PointLight>());
		engine.entityManager.addEntity(pointLight);


		auto tunnelSegment = Resources::loadModel("assets/models/tunnel-segment/tunnel_segment.gltf");
		for (int j = 0; j < tunnelSegment.size(); ++j) {
			auto& t = tunnelSegment[j];
			t->component<TransformComponent>().position = vec3(0, 0, -i * 20);
			engine.entityManager.addEntity(t);
		}

	}

	engine.entityManager.addEntity(sun);
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