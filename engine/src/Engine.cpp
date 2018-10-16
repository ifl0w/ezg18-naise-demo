#include <Engine.hpp>
#include <systems/RenderSystem.hpp>
#include <Resources.hpp>
#include <Logger.hpp>

#include <systems/EventManager.hpp>

#include <chrono>

using namespace NAISE::Engine;
using namespace std;
using namespace chrono;

EventManager Engine::eventManager;
SystemsManager Engine::systemsManager;
EntityManager Engine::entityManager;

Engine::Engine() {
	Engine::initialize();

	Engine::eventManager.event<RuntimeEvents::Quit>().subscribe([&](){
	  quit = true;
	});
}

Engine::~Engine() {
	Engine::shutdown();
}

void Engine::run() {
	while (!quit) {
		systemsManager.process(entityManager, _deltaTime);

		// fps calculation
		_deltaTime = duration_cast<microseconds>(steady_clock::now() - _lastFrame);
		_lastFrame = steady_clock::now();
		_fps = (uint32_t) (1000000 / _deltaTime.count());
		//NAISE_DEBUG_CONSOL("{}",_fps);
	}
}

EventManager& Engine::getEventManager() {
	return Engine::eventManager;
}

EntityManager& Engine::getEntityManager() {
	return Engine::entityManager;
}

SystemsManager& Engine::getSystemsManager() {
	return Engine::systemsManager;
}

void Engine::initialize() {
	Logger();
}

void Engine::shutdown() {
	Engine::systemsManager.cleanup();
	Engine::entityManager.cleanup();
	Resources::freeAll();
}
