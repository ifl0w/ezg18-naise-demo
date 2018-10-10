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

	void setInputMapper(shared_ptr<InputMapper> inputMapper);

private:
	Filter inputFilter;
	shared_ptr<InputMapper> _inputMapper;

	void _handleWindowEvents(SDL_Event& event);
};

}
}
