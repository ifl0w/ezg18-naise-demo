#include "CameraSelectionInputMapper.hpp"

#include <Engine.hpp>
#include <systems/PhysicsSystem.hpp>
#include <systems/RenderSystem.hpp>

using namespace NAISE::Engine;

void CameraSelectionInputMapper::handleEvent(const SDL_Event& event) {

	switch (event.type) {
	case SDL_KEYUP: {
		switch (event.key.keysym.sym) {
		case SDLK_SPACE: {
			auto entities = Engine::getEntityManager().getEntities<CameraSignature>();
			int activeIdx = 0;

			for (int i = 0; i < entities.size(); ++i) {
				auto& camComp = entities[i]->component<CameraComponent>();

				if (camComp.active) {
					activeIdx = i;
				}

				camComp.active = false;
			}

			if (activeIdx == entities.size() - 1) {
				activeIdx = 0;
			} else {
				activeIdx++;
			}

			entities[activeIdx]->component<CameraComponent>().active = true;

			break;
		}
		default:
			break;
		}
	}
	default:
		break;
	}
}