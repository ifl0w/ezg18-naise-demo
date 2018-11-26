#include "CameraSelectionSystem.hpp"
#include "CameraSelectionInputMapper.hpp"

#include <components/CameraComponent.hpp>
#include <systems/WindowSystem.hpp>

#include <Engine.hpp>

CameraSelectionSystem::CameraSelectionSystem() {
	Engine::getEntityManager().addSignature<SelectableCameraSignature>();
}

void CameraSelectionSystem::process(microseconds deltaTime) {

	for (auto entity: Engine::getEntityManager().getEntities<MovementSignature>()) {

	}

}
