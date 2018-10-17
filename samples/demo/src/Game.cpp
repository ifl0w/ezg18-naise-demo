#include <Engine.hpp>
#include <Logger.hpp>
#include <Resources.hpp>

#include <components/TransformComponent.hpp>
#include <components/MeshComponent.hpp>
#include <components/CameraComponent.hpp>
#include <components/MaterialComponent.hpp>
#include <components/InputComponent.hpp>
#include <components/LightComponent.hpp>
#include <components/ParentComponent.hpp>

#include <systems/render-engine/materials/PBRMaterial.hpp>
#include <systems/render-engine/lights/DirectionalLight.hpp>
#include <systems/render-engine/lights/PointLight.hpp>

#include <factories/MeshFactory.hpp>
#include <factories/LightFactory.hpp>
#include <factories/MaterialFactory.hpp>
#include <factories/RigidBodyFactory.hpp>

#include <systems/RenderSystem.hpp>
#include <systems/WindowSystem.hpp>
#include <systems/PhysicsSystem.hpp>
#include <systems/TransformSystem.hpp>

#include "Game.hpp"

#include "../../common/VisualDebugging/VisualDebuggingInputMapper.hpp"
#include "../../common/FPSCameraSystem/FPSCameraInputMapper.hpp"
#include "../../common/FPSCameraSystem/FPSCameraMovementSystem.hpp"

using namespace NAISE::Engine;

int main(int argc, char **argv) {
	NAISE::Engine::Engine engine;

	// initialize the systems of the engine
	Engine::getSystemsManager().registerSystem<WindowSystem>();
	Engine::getSystemsManager().registerSystem<InputSystem>();
	Engine::getSystemsManager().getSystem<InputSystem>().addInputMapper(make_shared<FPSCameraInputMapper>());
	Engine::getSystemsManager().getSystem<InputSystem>().addInputMapper(make_shared<VisualDebuggingInputMapper>());
	Engine::getSystemsManager().registerSystem<FPSCameraMovementSystem>();
	Engine::getSystemsManager().registerSystem<PhysicsSystem>();
	Engine::getSystemsManager().registerSystem<TransformSystem>();
	Engine::getSystemsManager().registerSystem<RenderSystem>();

	std::string posX = "assets/textures/skybox/clouds1_east.bmp";
	std::string negX = "assets/textures/skybox/clouds1_west.bmp";
	std::string posY = "assets/textures/skybox/clouds1_up.bmp";
	std::string negY = "assets/textures/skybox/clouds1_down.bmp";
	std::string posZ = "assets/textures/skybox/clouds1_north.bmp";
	std::string negZ = "assets/textures/skybox/clouds1_south.bmp";
	string identifier = "clouds1";

	std::vector<std::string> paths = {posX, negX, posY, negY, posZ, negZ};
	auto skybox = NAISE::Engine::Skybox(identifier, paths);
	//skybox.setBackgroundColor(glm::vec3(1,0.95,0.9));
	Engine::getSystemsManager().getSystem<RenderSystem>().setSkybox(skybox);

	auto sphere = make_shared<NAISE::Engine::Entity>();
	sphere->add<TransformComponent>();
	sphere->component<TransformComponent>().position = vec3(-2, 0, -5);
	sphere->add(RigidBodyFactory::createSphere(1, 10, vec3(-2, 0, -20)));
	sphere->add(MeshFactory::create<Sphere>());
	sphere->add<MaterialComponent>();
	sphere->add(MaterialFactory::createMaterial<PBRMaterial>(vec3(1, 0, 0), 0, 0.5));

	auto box = make_shared<NAISE::Engine::Entity>();
	box->add<TransformComponent>();
	box->component<TransformComponent>().position = vec3(0, -5, -5);
	box->component<TransformComponent>().scale = vec3(1, 1, 1);
	box->add(RigidBodyFactory::createBox(50, 1, 200, 0, vec3(0, -2, -5)));
	box->add(MeshFactory::createBox(50, 1, 200));
	//box->add(MaterialFactory::createMaterial<PBRMaterial>(vec3(0.2, 0.2, 0.2), 0, 0.6));
	auto materialComponent = make_shared<MaterialComponent>();
	auto material = make_shared<PBRMaterial>(vec3(0.2, 0.2, 0.2), 0, 0.6);
	material->displayGlowTexture = true;
	material->emissionTexture = Resources::loadTexture("glow", "assets/textures/skybox/clouds1_south.bmp");
	materialComponent->material = material;
	box->add(materialComponent);

	auto wall = make_shared<NAISE::Engine::Entity>();
	wall->add<TransformComponent>();
	wall->component<TransformComponent>().position = vec3(0, 5, -100);
	wall->add(RigidBodyFactory::createBox(50, 10, 1, 0, vec3(0, 5, -100)));
	wall->add(MeshFactory::createBox(50, 10, 1));
	wall->add(MaterialFactory::createMaterial<PBRMaterial>(vec3(0.2, 0.2, 0.2), 0, 0.6));
	wall->add<ParentComponent>(box->id);

	auto camera = make_shared<NAISE::Engine::Entity>();
	camera->add<TransformComponent>();
	camera->component<TransformComponent>().position = vec3(0, 1.6, 0);
	camera->add<CameraComponent>();
	camera->add<InputComponent>();
	camera->add(RigidBodyFactory::createSphere(1, 0, vec3(0, 0, 0), true));
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

	for (int i = 0; i < 5; ++i) {
		auto pointLight = make_shared<NAISE::Engine::Entity>();
		pointLight->add<TransformComponent>();
		pointLight->component<TransformComponent>().position = vec3(0, 0, -i * 20);
		pointLight->add(LightFactory::createLight<PointLight>());
		pointLight->component<LightComponent>().light->data.diffuse = vec4(30, 30, 30, 1);
		Engine::getEntityManager().addEntity(pointLight);

		auto tunnelSegment = Resources::loadModel("assets/models/tunnel-segment/tunnel_segment.gltf");
		for (int j = 0; j < tunnelSegment.size(); ++j) {
			auto &t = tunnelSegment[j];
			t->component<TransformComponent>().position = vec3(0, 0, -i * 20);
			Engine::getEntityManager().addEntity(t);
		}

	}

	auto luminarisScene = Resources::loadModel("assets/models/luminaris/luminaris.gltf");
	for (int j = 0; j < luminarisScene.size(); ++j) {
		auto &t = luminarisScene[j];
		t->component<TransformComponent>().position = vec3(0, 0, 25);
		t->component<TransformComponent>().scale = vec3(0.2);
		Engine::getEntityManager().addEntity(t);
	}

	Engine::getEntityManager().addEntity(sun);
	Engine::getEntityManager().addEntity(camera);
	Engine::getEntityManager().addEntity(sphere);
	Engine::getEntityManager().addEntity(box);
	Engine::getEntityManager().addEntity(wall);

//	engine.mainWindow->setResolution(1920, 1200);
//	engine.mainWindow->setFullscreen(false);
	engine.run();

	return 0;
}