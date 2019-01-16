#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>

#include "Event.hpp"
#include "scene/EntityManager.hpp"

#include "Logger.hpp"

namespace NAISE {
namespace Engine {

class System;
class Engine;

class SystemsManager {
	friend Engine;
public:
	template<typename T, typename ... Args>
	void registerSystem(Args... args);

	template<typename T>
	T& getSystem();

	template<typename T>
	void removeSystem();

	void process(std::chrono::microseconds deltaTime);

	void cleanup();

	std::unordered_map<string, float> getSystemTimes();
private:

	std::chrono::steady_clock::time_point _beginTime;

	std::unordered_map<std::type_index, std::chrono::duration<float, std::milli>> systemTimes;
	std::unordered_map<std::type_index, std::shared_ptr<System>> systems;
	std::vector<std::type_index> systemsInsertionOrder; // order is described by the position in the vector
};

template<typename T, typename... Args>
void SystemsManager::registerSystem(Args... args) {
	auto sys = std::make_shared<T>(args ...);
	auto type = std::type_index(typeid(T));
	systems.insert(pair(type, sys));
	systemsInsertionOrder.push_back(type);
}

template<typename T>
T& SystemsManager::getSystem() {
	auto idx = std::type_index(typeid(T));
	auto exits = systems.count(idx);

	if (!exits) {
		throw std::invalid_argument("SystemManager does not contain the requested system.");
	}

	auto tmp = static_cast<T*>(systems.at(idx).get());
	return *tmp;
}

template<typename T>
void SystemsManager::removeSystem() {
	NAISE_WARN_CONSOL("Remove system is untested! Functionality should be checked!");
	systems.erase(remove_if(systems.begin(), systems.end(),
					[](auto& s) { return typeid(s) == typeid(T); }));
	systemsInsertionOrder.erase(remove_if(systems.begin(), systems.end(),
										  [](auto& s) { return s == type_index(typeid(T)); }));
}

}
}
