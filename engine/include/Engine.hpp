#pragma once

#include "Window.hpp"

#include <scene/EntityManager.hpp>

#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <systems/System.hpp>
#include <systems/input/InputSystem.hpp>

using namespace std;

namespace NAISE {
namespace Engine {

class Engine {
public:
	Engine();

	void run();

	EntityManager entityManager;

	shared_ptr<InputSystem> inputSystem;

	shared_ptr<Window> mainWindow; // TODO: window will be deleted before the smart pointers to e.g. meshes => segfault

	/**
	 * @param system
	 */
	void addSystem(shared_ptr<System> system);

private:
	vector<shared_ptr<System>> systems;

	microseconds _deltaTime;
	steady_clock::time_point _lastFrame;
	uint32_t _fps;
};

}
}