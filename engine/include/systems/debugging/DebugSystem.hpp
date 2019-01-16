#pragma once

#include <systems/System.hpp>

#include <deque>

namespace NAISE {
namespace Engine {

namespace DebugEvents {
struct ToggleInterface: public Event<> {};
}


class DebugSystem: public System {
public:
	DebugSystem();

	void process(microseconds deltaTime) override;

private:
	bool _interfaceEnabled = false;

	std::vector<float> frameTimes = std::vector<float>(600);
};

}
}
