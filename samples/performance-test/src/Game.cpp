#include <Engine.hpp>

#include <components/TransformComponent.hpp>
#include <components/MeshComponent.hpp>
#include <components/CameraComponent.hpp>
#include <components/MaterialComponent.hpp>
#include <components/InputComponent.hpp>
#include <components/LightComponent.hpp>

#include <systems/render-engine/materials/PBRMaterial.hpp>
#include <systems/render-engine/lights/DirectionalLight.hpp>
#include <systems/render-engine/lights/PointLight.hpp>

#include <factories/MeshFactory.hpp>
#include <factories/LightFactory.hpp>
#include <factories/MaterialFactory.hpp>
#include <factories/RigidBodyFactory.hpp>

#include <Resources.hpp>

#include <systems/RenderSystem.hpp>
#include <systems/WindowSystem.hpp>
#include <systems/PhysicsSystem.hpp>
#include <systems/TransformSystem.hpp>

#include "Game.hpp"

#include "../../common/FPSCameraSystem/FPSCameraInputMapper.hpp"
#include "../../common/FPSCameraSystem/FPSCameraMovementSystem.hpp"

using namespace NAISE::Engine;

int main(int argc, char** argv) {
	NAISE::Engine::Engine engine;

	// initialize the systems of the engine
	Engine::getSystemsManager().registerSystem<WindowSystem>();
	Engine::getSystemsManager().registerSystem<InputSystem>();
	Engine::getSystemsManager().getSystem<InputSystem>().setInputMapper(make_shared<FPSCameraInputMapper>());
	Engine::getSystemsManager().registerSystem<FPSCameraMovementSystem>();
	Engine::getSystemsManager().registerSystem<PhysicsSystem>();
	Engine::getSystemsManager().registerSystem<TransformSystem>();
	Engine::getSystemsManager().registerSystem<RenderSystem>();

	auto floorEntity = make_shared<NAISE::Engine::Entity>();
	floorEntity->add<TransformComponent>();
	floorEntity->component<TransformComponent>().position = vec3(0, -2, -5);
	floorEntity->component<TransformComponent>().scale = vec3(1, 1, 1);
	floorEntity->add(RigidBodyFactory::createBox(200, 1, 200, 0, vec3(0, -2, -5)));
	floorEntity->add(MeshFactory::createBox(200, 1, 200));
	floorEntity->add(MaterialFactory::createMaterial<PBRMaterial>(vec3(0.8, 0.8, 0.8), 0, 0.2));

	auto camera = make_shared<NAISE::Engine::Entity>();
	camera->add<TransformComponent>();
	camera->component<TransformComponent>().position = vec3(0, 1.6, 0);
	camera->add<CameraComponent>();
	camera->add<InputComponent>();
	camera->add(RigidBodyFactory::createSphere(1, 0, vec3(0,0,0), true));
	camera->component<InputComponent>().add<Actions::MoveForward>();
	camera->component<InputComponent>().add<Actions::MoveBackward>();
	camera->component<InputComponent>().add<Actions::MoveLeft>();
	camera->component<InputComponent>().add<Actions::MoveRight>();
	camera->component<InputComponent>().add<Actions::MouseMotion>();
	camera->component<InputComponent>().add<Actions::MouseGrab>();

	auto sun = make_shared<NAISE::Engine::Entity>();
	sun->add<TransformComponent>();
	sun->add(LightFactory::createLight<DirectionalLight>());
	sun->component<LightComponent>().light->data.direction = vec4(-1, -2, -1, 1);
	sun->component<LightComponent>().light->data.ambient = vec4(0.75, 0.75, 0.75, 1);

	Engine::getEntityManager().addEntity(sun);
	Engine::getEntityManager().addEntity(camera);
	Engine::getEntityManager().addEntity(floorEntity);

	int amount = 10;
	float gapSize = 2;
	float radius = 1;
	vec3 cubeSize = vec3(amount * (radius * 2 + gapSize));
	vec3 positionOffset = vec3(0, cubeSize.z / 2.0 + 10, -(cubeSize.z / 2.0 + 50));
	for (int i = 0; i < amount; ++i) {
		for (int j = 0; j < amount; ++j) {
			for (int k = 0; k < amount; ++k) {
				auto sphere = make_shared<NAISE::Engine::Entity>();
				auto position = vec3(i * (radius * 2 + gapSize), j * (radius * 2 + gapSize), k * (radius * 2 + gapSize));
				position += (-cubeSize / 2.0f) + positionOffset;

				sphere->add<TransformComponent>();
				sphere->component<TransformComponent>().position = position;
				sphere->add(RigidBodyFactory::createSphere(radius, 10, position));
				sphere->add(MeshFactory::create<Sphere>(radius));
				sphere->add<MaterialComponent>();
				sphere->add(MaterialFactory::createMaterial<PBRMaterial>(vec3(0.8, 0, 0.8), 0, 0.2));

				Engine::getEntityManager().addEntity(sphere);
			}
		}
	}

//	engine.mainWindow->setResolution(1920, 1200);
//	engine.mainWindow->setFullscreen(false);
	engine.run();

	return 0;
}