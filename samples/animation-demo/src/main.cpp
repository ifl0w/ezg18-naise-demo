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
#include <resource-loader/GLTFLoader.hpp>

#include <systems/RenderSystem.hpp>
#include <systems/WindowSystem.hpp>
#include <systems/PhysicsSystem.hpp>
#include <systems/TransformSystem.hpp>
#include <systems/animations/AnimationSystem.hpp>

#include "../../common/FPSCameraSystem/FPSCameraInputMapper.hpp"
#include "../../common/FPSCameraSystem/FPSCameraMovementSystem.hpp"

using namespace NAISE::Engine;

int main(int argc, char** argv) {
	NAISE::Engine::Engine engine;

	// initialize the systems of the engine
	Engine::getSystemsManager().registerSystem<WindowSystem>();
	Engine::getSystemsManager().registerSystem<InputSystem>();
	Engine::getSystemsManager().getSystem<InputSystem>().addInputMapper(make_shared<FPSCameraInputMapper>());
	Engine::getSystemsManager().registerSystem<FPSCameraMovementSystem>();
	Engine::getSystemsManager().registerSystem<AnimationSystem>();
	Engine::getSystemsManager().registerSystem<TransformSystem>();
	Engine::getSystemsManager().registerSystem<RenderSystem>();
	
	std::string posX = "resources/textures/skybox/right.png";
	std::string negX = "resources/textures/skybox/left.png";
	std::string posY = "resources/textures/skybox/top.png";
	std::string negY = "resources/textures/skybox/bottom.png";
	std::string posZ = "resources/textures/skybox/front.png";
	std::string negZ = "resources/textures/skybox/back.png";
 	string identifier = "house";

	std::vector<std::string> paths = {posX, negX, posY, negY, posZ, negZ};
	auto skybox = NAISE::Engine::Skybox(identifier, paths);
	Engine::getSystemsManager().getSystem<RenderSystem>().setSkybox(skybox);

	int amount = 10;
	float gapSize = 0.01;
	float radius = 2;
	vec3 cubeSize = vec3(amount * (radius * 2 + gapSize));
	vec3 positionOffset = vec3(0, cubeSize.z / 2.0 + 10, -(cubeSize.z / 2.0 + 50));
	for (int i = 0; i < amount; ++i) {
		for (int j = 0; j < amount; ++j) {
			for (int k = 0; k < amount; ++k) {
				auto origin = make_shared<NAISE::Engine::Entity>();
				auto position = vec3(i * (radius * 2 + gapSize), j * (radius * 2 + gapSize), k * (radius * 2 + gapSize));
				position += (-cubeSize / 2.0f) + positionOffset;

				origin->add<TransformComponent>();
				origin->component<TransformComponent>().position = position;

				auto simpleCubeAnimation = GLTFLoader::loadModel("resources/simple-cube-animation.gltf");
				simpleCubeAnimation[0]->add<ParentComponent>(origin->id);
				simpleCubeAnimation[0]->component<TransformAnimationComponent>().animations[0].playing = true;
				simpleCubeAnimation[0]->component<TransformAnimationComponent>().animations[0].loopBehaviour = LOOP;

				Engine::getEntityManager().addEntity(origin);
				Engine::getEntityManager().addEntities(simpleCubeAnimation);
			}
		}
	}

	auto robotArm = GLTFLoader::loadModel("resources/simple-robot-arm-animation.gltf");
	for (auto& e: robotArm) {
		if(auto* c = e->get<TransformAnimationComponent>()) {
			for(auto& anim: c->animations) {
				if(anim.name == "ArmatureAction") {
					anim.playing = true;
					anim.loopBehaviour = LOOP;
				}
			}
		}

		if(auto* c = e->get<CameraComponent>()) {
			c->active = false;
		}
	}
	Engine::getEntityManager().addEntities(robotArm);

	auto box = make_shared<NAISE::Engine::Entity>();
	box->add<TransformComponent>();
	box->component<TransformComponent>().position = vec3(0, -2, -5);
	box->component<TransformComponent>().scale = vec3(1, 1, 1);
	box->add(MeshFactory::createBox(50, 1, 200));
	//box->add(MaterialFactory::createMaterial<PBRMaterial>(vec3(0.8, 0.8, 0.8), 0, 0.2));
	auto materialComponent = make_shared<MaterialComponent>();
	auto material = make_shared<PBRMaterial>(vec3(0.2, 0.2, 0.2), 0, 0.6);
	materialComponent->material = material;
	box->add(materialComponent);
	Engine::getEntityManager().addEntity(box);

	auto camera = make_shared<NAISE::Engine::Entity>();
	camera->add<TransformComponent>();
	camera->component<TransformComponent>().position = vec3(0, 1.6, 20);
	camera->add<CameraComponent>();
	camera->component<CameraComponent>().active = true;
	camera->add<InputComponent>();
	camera->component<InputComponent>().add<Actions::MoveForward>();
	camera->component<InputComponent>().add<Actions::MoveBackward>();
	camera->component<InputComponent>().add<Actions::MoveLeft>();
	camera->component<InputComponent>().add<Actions::MoveRight>();
	camera->component<InputComponent>().add<Actions::Sprint>();
	camera->component<InputComponent>().add<Actions::MouseMotion>();
	camera->component<InputComponent>().add<Actions::MouseGrab>();
	Engine::getEntityManager().addEntity(camera);

	auto sun = make_shared<NAISE::Engine::Entity>();
	sun->add<TransformComponent>();
	sun->add(LightFactory::createLight<DirectionalLight>());
	sun->component<LightComponent>().light->data.direction = vec4(-1, -2, -1, 1);
	sun->component<LightComponent>().light->data.ambient = vec4(0.75, 0.75, 0.75, 1);
	Engine::getEntityManager().addEntity(sun);

	engine.run();

	return 0;
}