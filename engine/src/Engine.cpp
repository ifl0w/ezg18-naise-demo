#include <Engine.hpp>
#include <systems/RenderSystem.hpp>

#include <chrono>

using namespace NAISE::Engine;
using namespace std;
using namespace chrono;

Engine::Engine()
		: mainWindow(make_shared<Window>()),
		  inputSystem(make_shared<InputSystem>()) {
	/*
	 * Use the logger "console" to log messages that are not suitable for the log file.
	 */
	auto console = spdlog::stdout_color_mt("console");
	console->set_level(spdlog::level::debug);

	inputSystem->setWindow(mainWindow);

	addSystem(inputSystem);
	addSystem(make_shared<RenderSystem>());
}

void Engine::run() {
	while (mainWindow->running) {
		for (auto& system: systems) {
			system->process(entityManager, _deltaTime);
		}

		SDL_GL_SwapWindow(mainWindow->window);

		// fps calculation
		_deltaTime = duration_cast<microseconds>(steady_clock::now() - _lastFrame);
		_lastFrame = steady_clock::now();
		_fps = (uint32_t) (1000000 / _deltaTime.count());
	}
}

void Engine::addSystem(shared_ptr<System> system) {
	systems.push_back(system);
}
