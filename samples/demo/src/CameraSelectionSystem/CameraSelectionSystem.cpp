#include <systems/RenderSystem.hpp>

#include "Engine.hpp"
#include "DemoEvents.hpp"

#include "CameraSelectionSystem.hpp"
#include "components/AnimationComponent.hpp"
#include "components/TagComponent.hpp"

#include "systems/animations/AnimationSystem.hpp"

#include "../../../common/VisualDebugging/VisualDebuggingInputMapper.hpp"
#include "../../../common/FPSCameraSystem/FPSCameraInputMapper.hpp"
#include "../../../common/FPSCameraSystem/FPSCameraMovementSystem.hpp"

using namespace NAISE::Engine;

CameraSelectionSystem::CameraSelectionSystem(CameraSequence cameraSequence) {
	_cameraSequence = cameraSequence;

	for (auto camSelection: _cameraSequence) {
		auto camTransform = Engine::getEntityManager().getEntity(camSelection.second.second);
		auto animComp = camTransform->get<TransformAnimationComponent>();

		for (auto& anim: animComp->animations) {
			anim.loopBehaviour = ONCE;
		}
	}

	if(!cameraSequence.empty()) {
		_selectedCamera = *cameraSequence.begin();
		auto cam = Engine::getEntityManager().getEntity(_selectedCamera.second.first);
		cam->get<CameraComponent>()->active = true;
	}

	Engine::getEventManager().event<CameraEvents::ToggleDebugCamera>().subscribe([&]() {
		debugCameraActive = !debugCameraActive;
	  	auto currentCam = Engine::getEntityManager().getEntity(_selectedCamera.second.first);

		if(debugCameraActive) {
			currentCam->component<CameraComponent>().active = false;
			debugCamera->get<CameraComponent>()->active = true;
		} else {
			currentCam->component<CameraComponent>().active = true;
			debugCamera->get<CameraComponent>()->active = false;
		}
	});

	Engine::getEventManager().event<CameraEvents::RestartDemo>().subscribe([&]() {
	  auto& animEntities = Engine::getEntityManager().getEntities<TransformAnimationSignature>();
	  for (auto& entity: animEntities) {
		  auto& animComp = entity->component<TransformAnimationComponent>();

		  for (auto& anim: animComp.animations) {
			  anim.reset();
			  anim.playing = true;
		  }
	  }

	  _selectedCamera = *_cameraSequence.begin();
	});

	Engine::getEventManager().event<CameraEvents::PauseDemo>().subscribe([&]() {
	  demoPaused = !demoPaused;

	  auto& animEntities = Engine::getEntityManager().getEntities<TransformAnimationSignature>();
	  for (auto& entity: animEntities) {
		  auto& animComp = entity->component<TransformAnimationComponent>();

		  for (auto& anim: animComp.animations) {
			  anim.playing = !demoPaused;
		  }
	  }
	});

	// Debug camera
	debugCamera = make_shared<NAISE::Engine::Entity>();
	debugCamera->add<TransformComponent>();
	debugCamera->component<TransformComponent>().position = vec3(10, 10, 50);
	debugCamera->component<TransformComponent>().rotation = quat(vec3(radians<float>(5), radians<float>(30), radians<float>(0)));
	debugCamera->add<CameraComponent>(glm::radians<float>(60), 0.1, 10000, 1024, 768);
	debugCamera->add<InputComponent>();
	debugCamera->component<InputComponent>().add<Actions::MoveForward>();
	debugCamera->component<InputComponent>().add<Actions::MoveBackward>();
	debugCamera->component<InputComponent>().add<Actions::MoveLeft>();
	debugCamera->component<InputComponent>().add<Actions::MoveRight>();
	debugCamera->component<InputComponent>().add<Actions::Sprint>();
	debugCamera->component<InputComponent>().add<Actions::MouseMotion>();
	debugCamera->component<InputComponent>().add<Actions::MouseGrab>();

	Engine::getEntityManager().addEntity(debugCamera);
}

void CameraSelectionSystem::process(microseconds deltaTime) {
	auto camTransform = Engine::getEntityManager().getEntity(_selectedCamera.second.second);
	auto animComp = camTransform->get<TransformAnimationComponent>();

	bool animationStopped = true;

	if (animComp) {
		for (auto& anim: animComp->animations) {
			if (!anim.ended()) {
				animationStopped = false;
			}
		}
	}

	if (animationStopped) {
		auto camera = Engine::getEntityManager().getEntity(_selectedCamera.second.first);
		camera->component<CameraComponent>().active = false;

		auto it = ++_cameraSequence.find(_selectedCamera.first);
		if (it == _cameraSequence.end()) {
			return;
		}

		_selectedCamera = *it;

		if (!debugCameraActive) {
			auto nextCam = Engine::getEntityManager().getEntity(_selectedCamera.second.first);
			nextCam->component<CameraComponent>().active = true;
		}
	}
}
