#include <Engine.hpp>
#include <Logger.hpp>
#include <Resources.hpp>
#include <resource-loader/GLTFLoader.hpp>

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

#include <debug/DebugInputMapper.hpp>

#include <systems/RenderSystem.hpp>
#include <systems/WindowSystem.hpp>
#include <systems/PhysicsSystem.hpp>
#include <systems/animations/AnimationSystem.hpp>
#include <systems/TransformSystem.hpp>
#include <systems/particle-system/SimpleGPUParticleSystem.hpp>

#include <systems/debugging/DebugSystem.hpp>
#include <systems/debugging/DebugInputMapper.hpp>

#include "Demo.hpp"
#include "SceneLoaderAdapter.hpp"
#include "CameraSelectionSystem/CameraSelectionInputMapper.hpp"
#include "CameraSelectionSystem/CameraSelectionSystem.hpp"

#include "../../common/VisualDebugging/VisualDebuggingInputMapper.hpp"
#include "../../common/FPSCameraSystem/FPSCameraInputMapper.hpp"
#include "../../common/FPSCameraSystem/FPSCameraMovementSystem.hpp"

using namespace NAISE::Engine;

int main(int argc, char **argv) {
	NAISE::Engine::Engine engine;

	// initialize the systems of the engine
	Engine::getSystemsManager().registerSystem<WindowSystem>();
	Engine::getSystemsManager().registerSystem<InputSystem>();

#ifdef DEBUG
	Engine::getSystemsManager().getSystem<InputSystem>().addInputMapper(make_shared<Debug::DebugInputMapper>());
#endif
	Engine::getSystemsManager().getSystem<InputSystem>().addInputMapper(make_shared<FPSCameraInputMapper>());
	Engine::getSystemsManager().getSystem<InputSystem>().addInputMapper(make_shared<VisualDebuggingInputMapper>());
	Engine::getSystemsManager().getSystem<InputSystem>().addInputMapper(make_shared<CameraSelectionInputMapper>());

	Engine::getSystemsManager().registerSystem<FPSCameraMovementSystem>();
	Engine::getSystemsManager().registerSystem<PhysicsSystem>();
	Engine::getSystemsManager().registerSystem<AnimationSystem>();
	Engine::getSystemsManager().registerSystem<TransformSystem>();
	Engine::getSystemsManager().registerSystem<SimpleGPUParticleSystem>();
	Engine::getSystemsManager().registerSystem<RenderSystem>();

	// Debugging interface
	Engine::getSystemsManager().registerSystem<DebugSystem>();
	Engine::getSystemsManager().getSystem<InputSystem>().addInputMapper(make_shared<DebugInputMapper>());

	std::string posX = "resources/textures/skybox/clouds1_east.bmp";
	std::string negX = "resources/textures/skybox/clouds1_west.bmp";
	std::string posY = "resources/textures/skybox/clouds1_up.bmp";
	std::string negY = "resources/textures/skybox/clouds1_down.bmp";
	std::string posZ = "resources/textures/skybox/clouds1_north.bmp";
	std::string negZ = "resources/textures/skybox/clouds1_south.bmp";
	string identifier = "clouds1";

	std::vector<std::string> paths = {posX, negX, posY, negY, posZ, negZ};
	auto skybox = NAISE::Engine::Skybox(identifier, paths);
	skybox.brightness = 5000;
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
	materialComponent->material = material;
	box->add(materialComponent);

	auto wall = make_shared<NAISE::Engine::Entity>();
	wall->add<TransformComponent>();
	wall->component<TransformComponent>().position = vec3(0, 5, -100);
	wall->add(RigidBodyFactory::createBox(50, 10, 1, 0, vec3(0, 5, -100)));
	wall->add(MeshFactory::createBox(50, 10, 1));
	wall->add(MaterialFactory::createMaterial<PBRMaterial>(vec3(0.2, 0.2, 0.2), 0, 0.6));
	wall->add<ParentComponent>(box->id);

	auto sun = make_shared<NAISE::Engine::Entity>();
	sun->add<TransformComponent>();
	sun->add(LightFactory::createLight<DirectionalLight>());
	sun->component<LightComponent>().light->data.direction = vec4(1, -2, -1, 1);
	sun->component<LightComponent>().light->data.diffuse = vec4(6000, 6000, 6000, 1);
	sun->component<LightComponent>().light->data.ambient = vec4(200); // TODO: adapt GLTF Loader default parameter for radius calculation

	auto platformParticles = make_shared<NAISE::Engine::Entity>();
	platformParticles->add<TransformComponent>();
	platformParticles->component<TransformComponent>().position = vec3(-1.2, 1.2, 2);
	platformParticles->add<SimpleGPUParticleComponent>("resources/particle-systems/platform-particles.glsl", 10000, 1000);
	platformParticles->component<SimpleGPUParticleComponent>().setUniforms = [platformParticles](gl::GLuint shaderProgram){
	  glProgramUniform1f(shaderProgram, uniformLocation(shaderProgram, "uRadius"), 10.0f);
	  glProgramUniform3fv(shaderProgram, uniformLocation(shaderProgram, "uVelocity"), 1, value_ptr(vec3(0, 0.25, 0)));
	  glProgramUniform2fv(shaderProgram, uniformLocation(shaderProgram, "uLifeTime"), 1, value_ptr(vec2(2, 7)));
	};
	platformParticles->add<MeshParticleComponent>(10000);
	platformParticles->component<MeshParticleComponent>().mesh = Resources::getMesh<Sphere>("particle", 0.025, 4, 3);
	auto particleMaterial = Resources::getMaterial<PBRMaterial>("ParticleGlow");
	particleMaterial->glow = vec3(0,1,1);
	platformParticles->component<MeshParticleComponent>().material = particleMaterial;
	platformParticles->add<AABBComponent>();

	// load config file
	json config = Resources::loadConfig("resources/config.json");

	// load scenes
	SceneLoaderAdapter loaderAdapter;
	for (json& scene: config["scenes"]) {
		auto sceneEntities = GLTFLoader::loadModel(&loaderAdapter, scene["path"].get<std::string>());
		Engine::getEntityManager().addEntities(sceneEntities);
	}

	// kick off animations
	auto& animEntities = Engine::getEntityManager().getEntities<TransformAnimationSignature>();
	for (auto& entity: animEntities) {
		auto& animComp = entity->component<TransformAnimationComponent>();
		auto& transformComp = entity->component<TransformComponent>();

		for (auto& anim: animComp.animations) {
			anim.playing = true;
			anim.loopBehaviour = LOOP;
		}
	}

	Engine::getSystemsManager().registerSystem<CameraSelectionSystem>(loaderAdapter.cameraSequence);

	Engine::getEntityManager().addEntity(platformParticles);
	Engine::getEntityManager().addEntity(sun);

	// setup video settings
	initVideoSettings(config);

	engine.run();

	return 0;
}

void initVideoSettings(json config) {
	auto width = 1024;
	auto height = 768;
	auto fullscreen = false;

	if (config["window"].is_object()) {
		try {
			width = config["window"].value("width", 1024);
			height = config["window"].value("height", 768);
			fullscreen = config["window"].value("fullscreen", false);
		} catch (nlohmann::detail::type_error& e) {
			NAISE_WARN_LOG("Video config invalid! Using default values.")
		}

	}

	NAISE_INFO_CONSOL("Window dimensions: {} x {}, Fullscreen: {}", width, height, fullscreen)

	Engine::getEventManager().event<WindowEvents::SetResolution>().emit(width, height);
	Engine::getEventManager().event<WindowEvents::SetFullscreen>().emit(fullscreen);

	// TODO: find better solution for adapting camera aspect rations (z.B. event in RenderSystem or CameraSystem)
	// apply resolution to all cameras
	for(auto e: Engine::getEntityManager().getEntities<CameraSignature>()) {
		e->component<CameraComponent>().setAspectRatio(width, height);
	}
}
