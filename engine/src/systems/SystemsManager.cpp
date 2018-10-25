#include <systems/SystemsManager.hpp>

#include <systems/System.hpp>
#include <Logger.hpp>

using namespace NAISE::Engine;

void SystemsManager::process(const EntityManager& em, std::chrono::microseconds deltaTime) {
//	NAISE_DEBUG_CONSOL("### Start processing of systems ###");
	for (const auto& system: systems) {
		_beginTime = steady_clock::now();

		system.second->process(deltaTime);

		_deltaTime = duration_cast<microseconds>(steady_clock::now() - _beginTime);
//		NAISE_DEBUG_CONSOL("{} :: Processing took {}ms", system.first.name(), duration_cast<milliseconds>(_deltaTime).count());
	}
//	NAISE_DEBUG_CONSOL("### End processing of systems ###");
}

void SystemsManager::cleanup() {
	systems.clear();
}

