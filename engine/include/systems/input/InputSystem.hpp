#pragma once

#include <Window.hpp>
#include <filter/Filter.hpp>

#include "systems/System.hpp"
#include "InputMapper.hpp"

namespace NAISE {
namespace Engine {

class InputSystem : public System {
public:
	InputSystem();

	void process(const EntityManager& em, microseconds deltaTime) override;

	void setWindow(shared_ptr<Window> window);

	void setInputMapper(shared_ptr<InputMapper> inputMapper);

protected:
	shared_ptr<Window> _window;

private:
	Filter inputFilter;
	shared_ptr<InputMapper> _inputMapper;
};

}
}
