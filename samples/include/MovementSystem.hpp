#pragma once

#include <components/InputComponent.hpp>
#include <components/TransformComponent.hpp>
#include <systems/System.hpp>

using namespace NAISE::Engine;

class MovementSystem : public System {
public:
	MovementSystem() {
		movementFilter.requirement<InputComponent>();
		movementFilter.requirement<TransformComponent>();
	}

	void process(const EntityManager& em, microseconds deltaTime) override;

private:
	Filter movementFilter;
};
