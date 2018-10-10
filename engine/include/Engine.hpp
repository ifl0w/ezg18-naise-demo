#pragma once

#include <scene/EntityManager.hpp>

#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <systems/System.hpp>
#include <systems/input/InputSystem.hpp>

using namespace std;

namespace NAISE {
namespace Engine {

namespace RuntimeEvents {
struct Quit: public Event<> {};
}

class Engine {
public:
	Engine();
	~Engine();

	void run();

	EntityManager entityManager;
	SystemsManager systemsManager;

private:
	microseconds _deltaTime;
	steady_clock::time_point _lastFrame;
	uint32_t _fps;

	bool quit = false;
};

}
}