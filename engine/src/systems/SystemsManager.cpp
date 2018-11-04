#include <systems/SystemsManager.hpp>

#include <systems/System.hpp>
#include <Logger.hpp>

using namespace NAISE::Engine;
using namespace std::chrono;

void SystemsManager::process(std::chrono::microseconds deltaTime) {
//	NAISE_DEBUG_CONSOL("### Start processing of systems ###");
	for (const auto& type: systemsInsertionOrder) {
		_beginTime = steady_clock::now();

		systems[type]->process(deltaTime); // process in correct order

		_deltaTime = duration_cast<microseconds>(steady_clock::now() - _beginTime);
//		NAISE_DEBUG_CONSOL("{} :: Processing took {}ms", type.name(), duration_cast<milliseconds>(_deltaTime).count());
	}
//	NAISE_DEBUG_CONSOL("### End processing of systems ###");
}

void SystemsManager::cleanup() {
	for_each(systemsInsertionOrder.rbegin(), systemsInsertionOrder.rend(), [&](const auto& type){
	  auto it = systems.find(type);
	  systems.erase(it);
	});

	systemsInsertionOrder.clear();
	systems.clear();
}

