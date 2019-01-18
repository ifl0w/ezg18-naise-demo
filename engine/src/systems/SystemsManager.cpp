#include <systems/SystemsManager.hpp>

#include <systems/System.hpp>
#include <Logger.hpp>

using namespace NAISE::Engine;
using namespace std::chrono;

void SystemsManager::process(std::chrono::microseconds deltaTime) {
	for (const auto& type: systemsInsertionOrder) {
		_beginTime = steady_clock::now();

		systems[type]->process(deltaTime); // process in correct order

		systemTimes[type] = duration_cast<milliseconds>(steady_clock::now() - _beginTime);
	}
}

void SystemsManager::cleanup() {
	for_each(systemsInsertionOrder.rbegin(), systemsInsertionOrder.rend(), [&](const auto& type){
	  auto it = systems.find(type);
	  systems.erase(it);
	});

	systemsInsertionOrder.clear();
	systems.clear();
}

std::unordered_map<string, float> SystemsManager::getSystemTimes() {
	unordered_map<string, float> ret = unordered_map<string, float>(systemTimes.size());

	for(auto& t: systemTimes) {
		ret[t.first.name()] = t.second.count();
	}

	return ret;
}

