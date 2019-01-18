#include <systems/debugging/DebugInputMapper.hpp>

#include <Engine.hpp>


using namespace NAISE::Engine;

void DebugInputMapper::handleEvent(const SDL_Event& event) {
	switch (event.type) {
	case SDL_KEYUP: {
		switch (event.key.keysym.sym) {
		case SDLK_F8: {
			Engine::getEventManager().event<DebugEvents::ToggleInterface>().emit();
			break;
		}

			break;
		}
	}
	}
}
