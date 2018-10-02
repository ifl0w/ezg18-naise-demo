#include <Engine.hpp>
#include <scene/Entity.hpp>
#include <systems/RenderSystem.hpp>

using namespace NAISE::Engine;
using namespace std;

Engine::Engine() {
	/*
	 * Use the logger "console" to log messages that are not suitable for the log file.
	 */
	auto console = spdlog::stdout_color_mt("console");
	console->set_level(spdlog::level::debug);

	addSystem(make_shared<RenderSystem>());
}

void Engine::run() {
	while (mainWindow.running) {
		pollEvents(mainWindow);

		for (auto& system: systems) {
			system->process(entityManager);
		}

		SDL_GL_SwapWindow(mainWindow.window);
	}
}

void Engine::pollEvents(Window& window) {
	SDL_Event event; /* Event structure */

	while (SDL_PollEvent(&event)) {
		window.handleEvent(event);
	}
}

void Engine::addSystem(shared_ptr<System> system) {
	systems.push_back(system);
}
