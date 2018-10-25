#pragma once

#include <components/InputComponent.hpp>
#include <components/TransformComponent.hpp>
#include <systems/System.hpp>

using namespace NAISE::Engine;

struct MovementSignature: public Signature<InputComponent, TransformComponent> {};

class FPSCameraMovementSystem : public System {
public:
	FPSCameraMovementSystem();

	void process(microseconds deltaTime) override;

private:
	float _moveSpeed = 5;
	float _sprintMultiplier = 5;
	float _mouseSpeed = 0.1;
};
