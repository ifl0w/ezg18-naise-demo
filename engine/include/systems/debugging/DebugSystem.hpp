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

	float minimumPlotFrameTime = 50; // in ms
	float accumFrameTime = 0; // in ms
	float frameTimeSamples = 0;
	void renderProfilingInterface(microseconds deltaTime);
	std::vector<float> frameTimes = std::vector<float>(100);
};

}
}
