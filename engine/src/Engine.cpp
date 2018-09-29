#include <Engine.hpp>
#include <scene/Entity.hpp>

using namespace NAISE::Engine;
using namespace std;

Engine::Engine() {
	/*
	 * Use the logger "console" to log messages that are not suitable for the log file.
	 */
	auto console = spdlog::stdout_color_mt("console");
	console->set_level(spdlog::level::debug);


}

void Engine::run() {
	while (mainWindow.running) {
		pollEvents(mainWindow);
	}
}

void Engine::pollEvents(Window& window) {
	SDL_Event event; /* Event structure */

	while (SDL_PollEvent(&event)) {
		window.handleEvent(event);
	}
}

