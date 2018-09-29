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

private:
	Window mainWindow;

	void pollEvents(Window& window);
};

}
}