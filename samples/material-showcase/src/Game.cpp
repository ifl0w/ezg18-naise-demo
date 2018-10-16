#include <Engine.hpp>

#include <components/TransformComponent.hpp>
#include <components/MeshComponent.hpp>
#include <components/CameraComponent.hpp>
#include <components/MaterialComponent.hpp>
#include <components/InputComponent.hpp>
#include <components/LightComponent.hpp>
#include <Logger.hpp>

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
	Engine::getSystemsManager().registerSystem<RenderSystem>();
	
	std::string posX = "assets/textures/skybox/right.png";
	std::string negX = "assets/textures/skybox/left.png";
	std::string posY = "assets/textures/skybox/top.png";
	std::string negY = "assets/textures/skybox/bottom.png";
	std::string posZ = "assets/textures/skybox/front.png";
	std::string negZ = "assets/textures/skybox/back.png";
 	string identifier = "house";

	std::vector<std::string> paths = {posX, negX, posY, negY, posZ, negZ};
	auto skybox = NAISE::Engine::Skybox(identifier, paths);
	Engine::getSystemsManager().getSystem<RenderSystem>().setSkybox(skybox);

	int resolution = 10;
	float radius = 1;
	float gap = 0.5;
	vec3 color = vec3(0.8, 0, 0);
	for (int i = 0; i <= resolution; ++i) {
		for (int j = 0; j <= resolution; ++j) {
			auto sphere = make_shared<NAISE::Engine::Entity>();
			sphere->add<TransformComponent>();
			sphere->component<TransformComponent>().position = vec3(j*(2*radius+gap), i*(2*radius+gap), -40);
			sphere->add(MeshFactory::create<Sphere>(radius));
			sphere->add<MaterialComponent>();
			sphere->add(MaterialFactory::createMaterial<PBRMaterial>(color, i/(float)resolution, j/(float)resolution));
			Engine::getEntityManager().addEntity(sphere);
		}
	}

	auto box = make_shared<NAISE::Engine::Entity>();
	box->add<TransformComponent>();
	box->component<TransformComponent>().position = vec3(0, -2, -5);
	box->component<TransformComponent>().scale = vec3(1, 1, 1);
	box->add(MeshFactory::createBox(50, 1, 200));
	//box->add(MaterialFactory::createMaterial<PBRMaterial>(vec3(0.8, 0.8, 0.8), 0, 0.2));

	auto materialComponent = make_shared<MaterialComponent>();
	auto material = make_shared<PBRMaterial>(vec3(0.2, 0.2, 0.2), 0, 0.6);
/*	material->albedoTexture = Resources::loadTexture("bla4", "assets/textures/cb7lf-1haxn.dds");
	material->albedoTexture = Resources::loadTexture("bla3", "assets/textures/clouds1_west.bmp");
	material->albedoTexture = Resources::loadTexture("bla2", "assets/textures/skybox/top.png");
	material->albedoTexture = Resources::loadTexture("bla5", "assets/textures/horizontalSkyBox.png");

	material->albedoTexture = Resources::loadTexture("color", "assets/textures/Metal_Panel_005_COLOR.jpg");
	material->normalTexture = Resources::loadTexture("norm", "assets/textures/Metal_Panel_005_NORM.jpg");
	material->metallicRoughnessTexture = Resources::loadTexture("rough", "assets/textures/Metal_Panel_005_ROUGH.jpg");*/
	materialComponent->material = material;
	box->add(materialComponent);

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
	camera->component<InputComponent>().add<Actions::MouseGrab>();

	auto sun = make_shared<NAISE::Engine::Entity>();
	sun->add<TransformComponent>();
	sun->add(LightFactory::createLight<DirectionalLight>());
	sun->component<LightComponent>().light->data.direction = vec4(-1, -2, -1, 1);
	sun->component<LightComponent>().light->data.ambient = vec4(0.75, 0.75, 0.75, 1);

	Engine::getEntityManager().addEntity(sun);
	Engine::getEntityManager().addEntity(camera);
	Engine::getEntityManager().addEntity(box);

	engine.run();
}