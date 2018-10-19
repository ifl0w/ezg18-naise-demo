#pragma once

#include <components/InputComponent.hpp>
#include <components/TransformComponent.hpp>
#include <systems/System.hpp>

using namespace NAISE::Engine;

class FPSCameraMovementSystem : public System {
public:
	FPSCameraMovementSystem() {
		movementFilter.requirement<InputComponent>();
		movementFilter.requirement<TransformComponent>();
	}

	void process(const EntityManager& em, microseconds deltaTime) override;

private:
	Filter movementFilter;

	float _moveSpeed = 5;
	float _sprintMultiplier = 5;
	float _mouseSpeed = 0.1;
};
