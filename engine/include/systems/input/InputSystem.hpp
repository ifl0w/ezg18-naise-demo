#pragma once

#include <filter/Filter.hpp>

#include "systems/System.hpp"
#include "InputMapper.hpp"

#include <components/InputComponent.hpp>

namespace NAISE {
namespace Engine {

struct InputSignature: public Signature<InputComponent> {};

class InputSystem : public System {
public:
	InputSystem();

	void process(microseconds deltaTime) override;

	void addInputMapper(shared_ptr<InputMapper> inputMapper);

private:
	std::vector<shared_ptr<InputMapper>> _inputMapper;

	void _handleWindowEvents(SDL_Event& event);
};

}
}
