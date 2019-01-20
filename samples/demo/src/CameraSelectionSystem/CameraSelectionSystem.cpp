#include <systems/RenderSystem.hpp>

#include "Engine.hpp"

#include "CameraSelectionSystem.hpp"

using namespace NAISE::Engine;

CameraSelectionSystem::CameraSelectionSystem(nlohmann::json cameraConfig) {

	if(cameraConfig.is_array()) {

	}

}

void CameraSelectionSystem::process(microseconds deltaTime) {
	accumTime += deltaTime;

	auto entities = Engine::getEntityManager().getEntities<CameraSignature>();
	for(auto& e: entities) {
		if (auto camComp = e->get<CameraComponent>()) {

		}
	}

}
