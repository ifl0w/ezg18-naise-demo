#include <Engine.hpp>

#include <systems/input/InputSystem.hpp>
#include <components/InputComponent.hpp>

using namespace NAISE::Engine;

InputSystem::InputSystem(): _inputMapper(make_shared<InputMapper>()) {
	inputFilter.requirement<InputComponent>();
}

void InputSystem::process(const EntityManager& em, microseconds deltaTime) {
	SDL_Event event; /* Event structure */

	while (SDL_PollEvent(&event)) {
		// handle window events (e.g. window close)
		_handleWindowEvents(event);

		_inputMapper->handleEvent(event, _systemsManager);

		auto actions = _inputMapper->resolve(event);

		em.filter(inputFilter, [=](Entity& entity) {
		  // handle remaining events
		  auto& comp = entity.component<InputComponent>();

		  for (auto& action: actions) {
			  if (comp.hasAction(action)) {
				  _inputMapper->input(action, comp.action(action), event);
			  }
		  }
		});
	}
}

void InputSystem::setInputMapper(shared_ptr<InputMapper> inputMapper) {
	_inputMapper = inputMapper;
}

void InputSystem::_handleWindowEvents(SDL_Event& event) {
	if (event.type == SDL_WINDOWEVENT) {
		switch (event.window.event) {
		case SDL_WINDOWEVENT_CLOSE:
			_systemsManager->event<RuntimeEvents::Quit>().emit();
			break;
		default:
			// swallow unhandled events
			break;
		}
	}
}