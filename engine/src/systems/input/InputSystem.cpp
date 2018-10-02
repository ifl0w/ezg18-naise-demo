#include <utility>

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
		_window->handleEvent(event);

		em.filter(inputFilter, [=](Entity& entity) {
		  // handle remaining events
		  auto actions = _inputMapper->resolve(event);
		  auto& comp = entity.component<InputComponent>();

		  for (auto& action: actions) {
			  if (comp.hasAction(action)) {
				  comp.mapping[action] = _inputMapper->input(action, event);
			  }
		  }
		});
	}
}

void InputSystem::setWindow(shared_ptr<Window> window) {
	_window = std::move(window);
}

void InputSystem::setInputMapper(shared_ptr<InputMapper> inputMapper) {
	_inputMapper = inputMapper;
}
