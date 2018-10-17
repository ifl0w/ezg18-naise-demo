#include "VisualDebuggingInputMapper.hpp"

#include <Engine.hpp>
#include <systems/PhysicsSystem.hpp>
#include <systems/RenderSystem.hpp>

using namespace NAISE::Engine;

void VisualDebuggingInputMapper::handleEvent(const SDL_Event& event) {

	switch (event.type) {
	case SDL_KEYUP: {
		switch (event.key.keysym.sym) {
		case SDLK_F8:
		{
			try {
				auto& physicsSystem = Engine::getSystemsManager().getSystem<PhysicsSystem>();
				physicsSystem.toggleVisualDebugging();
			} catch (const std::invalid_argument& e) {
				// swallow and forget
				NAISE_DEBUG_CONSOL("No physics system in use.")
			}

			break;
		}
		case SDLK_ESCAPE:
			Engine::getEventManager().event<RuntimeEvents::Quit>().emit();
			break;
		}

		break;
	}
	}
}
