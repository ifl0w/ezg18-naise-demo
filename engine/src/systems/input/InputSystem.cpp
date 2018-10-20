#include <Engine.hpp>

#include <systems/input/InputSystem.hpp>
#include <components/InputComponent.hpp>

using namespace NAISE::Engine;

InputSystem::InputSystem() {
	Engine::getEntityManager().addSignature<InputSignature>();
}

void InputSystem::process(const EntityManager& em, microseconds deltaTime) {
	SDL_Event event; /* Event structure */

	while (SDL_PollEvent(&event)) {
		// handle window events (e.g. window close)
		_handleWindowEvents(event);

		for (const auto& mapper: _inputMapper) {
			mapper->handleEvent(event);

			auto actions = mapper->resolve(event);

			for (auto entity: Engine::getEntityManager().getSignature<InputSignature>()->entities) {
				// handle remaining events
				auto& comp = entity->component<InputComponent>();

				for (auto& action: actions) {
					if (comp.hasAction(action)) {
						mapper->input(action, comp.action(action), event);
					}
				}
			}
		}
	}
}

void InputSystem::addInputMapper(shared_ptr<InputMapper> inputMapper) {
	_inputMapper.push_back(inputMapper);
}

void InputSystem::_handleWindowEvents(SDL_Event& event) {
	if (event.type == SDL_WINDOWEVENT) {
		switch (event.window.event) {
		case SDL_WINDOWEVENT_CLOSE:
			Engine::getEventManager().event<RuntimeEvents::Quit>().emit();
			break;
		default:
			// swallow unhandled events
			break;
		}
	}
}