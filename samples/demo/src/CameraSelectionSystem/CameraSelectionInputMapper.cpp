#include "CameraSelectionInputMapper.hpp"
#include "DemoEvents.hpp"

#include <Engine.hpp>
#include <systems/PhysicsSystem.hpp>
#include <systems/RenderSystem.hpp>

using namespace NAISE::Engine;

void CameraSelectionInputMapper::handleEvent(const SDL_Event& event) {

	switch (event.type) {
	case SDL_KEYUP: {
		switch (event.key.keysym.sym) {
		case SDLK_SPACE: {
			Engine::getEventManager().event<CameraEvents::ToggleDebugCamera>().emit();
			break;
		}
		case SDLK_BACKSPACE: {
			Engine::getEventManager().event<CameraEvents::RestartDemo>().emit();
			break;
		}
		case SDLK_F1: {
			Engine::getEventManager().event<CameraEvents::PauseDemo>().emit();
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