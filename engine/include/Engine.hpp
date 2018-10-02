#pragma once

#include "Window.hpp"

#include <scene/EntityManager.hpp>

#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

using namespace std;

namespace NAISE {
namespace Engine {

class Engine {
public:
	Engine();

	void run();

	EntityManager entityManager;

	/**
	 * @param system
	 */
	void addSystem(shared_ptr<System> system);

private:
	Window mainWindow; // TODO: window will be deleted before the smart pointers to e.g. meshes => segfault

	void pollEvents(Window& window);

	vector<shared_ptr<System>> systems;
};

}
}