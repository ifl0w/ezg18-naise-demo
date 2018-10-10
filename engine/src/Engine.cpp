#include <Engine.hpp>
#include <systems/RenderSystem.hpp>
#include <Resources.hpp>

#include <chrono>

using namespace NAISE::Engine;
using namespace std;
using namespace chrono;

Engine::Engine() {
	/*
	 * Use the logger "console" to log messages that are not suitable for the log file.
	 */
	auto console = spdlog::stdout_color_mt("console");
	console->set_level(spdlog::level::debug);

	auto logger = spdlog::stdout_color_mt("logger");
	logger->set_level(spdlog::level::warn);

	systemsManager.registerEvent<RuntimeEvents::Quit>().subscribe([&](){
	  quit = true;
	});
}

Engine::~Engine() {
	Resources::freeAll();
}

void Engine::run() {
	while (!quit) {
		systemsManager.process(entityManager, _deltaTime);

		// fps calculation
		_deltaTime = duration_cast<microseconds>(steady_clock::now() - _lastFrame);
		_lastFrame = steady_clock::now();
		_fps = (uint32_t) (1000000 / _deltaTime.count());
//		spdlog::get("console")->debug(_fps);
	}
}
