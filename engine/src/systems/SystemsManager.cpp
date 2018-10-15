#include <systems/SystemsManager.hpp>

#include <systems/System.hpp>

using namespace NAISE::Engine;

void SystemsManager::process(const EntityManager& em, std::chrono::microseconds deltaTime) {
	for (const auto& system: systems) {
		system.second->process(em, deltaTime);
	}
}

void SystemsManager::cleanup() {
	systems.clear();
}

