#include "FPSCameraMovementSystem.hpp"
#include "FPSCameraInputMapper.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#include <components/CameraComponent.hpp>
#include <systems/WindowSystem.hpp>

#include <Engine.hpp>

FPSCameraMovementSystem::FPSCameraMovementSystem() {
	Engine::getEntityManager().addSignature<MovementSignature>();
}

void FPSCameraMovementSystem::process(const NAISE::Engine::EntityManager& em, microseconds deltaTime) {

	for (auto entity: Engine::getEntityManager().getSignature<MovementSignature>()->entities) {
		std::chrono::duration<float> sec = deltaTime;

		auto& inputComponent = entity->component<InputComponent>();
		auto& transform = entity->component<TransformComponent>();

		float moveSpeed = _moveSpeed;
		float mouseSpeed = _mouseSpeed;

		if (inputComponent.hasAction<Actions::Sprint>()) {
			auto& inp = inputComponent.action<Actions::Sprint>();
			bool active = inp.get<bool>("active");

			if (active) {
				moveSpeed = _moveSpeed * _sprintMultiplier;
			}
		}

		if (inputComponent.hasAction<Actions::MoveForward>()) {
			auto& inp = inputComponent.action<Actions::MoveForward>();
			bool active = inp.get<bool>("active");

			if (active) {
				vec3 orientationAxis = axis(transform.rotation);
				vec3 movement = vec3(0, 0, -moveSpeed * sec.count()) / transform.globalScale;
				transform.position += rotate(movement, angle(transform.rotation), orientationAxis);
			}
		}

		if (inputComponent.hasAction<Actions::MoveBackward>()) {
			auto& inp = inputComponent.action<Actions::MoveBackward>();
			bool active = inp.get<bool>("active");

			if (active) {
				vec3 orientationAxis = axis(transform.rotation);
				vec3 movement = vec3(0, 0, moveSpeed * sec.count()) / transform.globalScale;
				transform.position += rotate(movement, angle(transform.rotation), orientationAxis);
			}
		}

		if (inputComponent.hasAction<Actions::MoveLeft>()) {
			auto& inp = inputComponent.action<Actions::MoveLeft>();
			bool active = inp.get<bool>("active");

			if (active) {
				vec3 orientationAxis = axis(transform.rotation);
				vec3 movement = vec3(-moveSpeed * sec.count(), 0, 0) / transform.globalScale;
				transform.position += rotate(movement, angle(transform.rotation), orientationAxis);
			}
		}

		if (inputComponent.hasAction<Actions::MoveRight>()) {
			auto& inp = inputComponent.action<Actions::MoveRight>();
			bool active = inp.get<bool>("active");

			if (active) {
				vec3 orientationAxis = axis(transform.rotation);
				vec3 movement = vec3(moveSpeed * sec.count(), 0, 0) / transform.globalScale;
				transform.position += rotate(movement, angle(transform.rotation), orientationAxis);
			}
		}

		if (inputComponent.hasAction<Actions::MouseGrab>()) {
			auto& inp = inputComponent.action<Actions::MouseGrab>();
			bool active = inp.get<bool>("active");

			if (active != inp.get<bool>("grabbed")) {
				Engine::getEventManager().event<WindowEvents::CaptureMouse>().emit(active);
				inp.set("grabbed", active);
			}

			if (active) {
				if (inputComponent.hasAction<Actions::MouseMotion>()) {
					auto& inp = inputComponent.action<Actions::MouseMotion>();

					if (inp.get<bool>("active")) {
						float deltaX = inp.get<int>("delta_x");
						float deltaY = inp.get<int>("delta_y");
						inp.set("delta_x", 0);
						inp.set("delta_y", 0);
						inp.set("active", false);

						deltaX *= mouseSpeed;
						deltaY *= mouseSpeed;

						// https://gamedev.stackexchange.com/questions/30644/how-to-keep-my-quaternion-using-fps-camera-from-tilting-and-messing-up
						quat pitchQuat = quat(vec3(radians<float>(-deltaY), 0, 0));
						quat yawQuat = quat(vec3(0, radians<float>(-deltaX), 0));
						transform.rotation = yawQuat * transform.rotation * pitchQuat;

						// clamp value because: https://github.com/g-truc/glm/issues/820
						transform.rotation.w = glm::clamp<float>(transform.rotation.w, -1, 1);
					}
				}
			}
		}

		if (inputComponent.hasAction<Actions::MouseMotion>()) {
			auto& inp = inputComponent.action<Actions::MouseMotion>();

			inp.set("delta_x", 0);
			inp.set("delta_y", 0);
		}

		// Recalculate camera frustum.
		// This has to be done always after camera movement.
		entity->component<CameraComponent>().frustum.recalculate(transform.getModelMatrix());
	}

}
