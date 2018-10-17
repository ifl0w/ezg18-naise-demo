#pragma once

#include <filter/Filter.hpp>

#include "systems/System.hpp"
#include "InputMapper.hpp"

namespace NAISE {
namespace Engine {

class InputSystem : public System {
public:
	InputSystem();

	void process(const EntityManager& em, microseconds deltaTime) override;

	void addInputMapper(shared_ptr<InputMapper> inputMapper);

private:
	Filter inputFilter;
	std::vector<shared_ptr<InputMapper>> _inputMapper;

	void _handleWindowEvents(SDL_Event& event);
};

}
}
