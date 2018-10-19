#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>

#include "Event.hpp"
#include "scene/EntityManager.hpp"

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

	void process(const EntityManager& em, std::chrono::microseconds deltaTime);

	void cleanup();
private:

	std::chrono::microseconds _deltaTime = std::chrono::microseconds(0);
	std::chrono::steady_clock::time_point _beginTime;

	std::unordered_map<std::type_index, std::shared_ptr<System>> systems;
};

template<typename T, typename... Args>
void SystemsManager::registerSystem(Args... args) {
	auto s = std::make_shared<T>(args ...);
	systems.insert(pair(std::type_index(typeid(T)), s));
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
	erase(remove_if(systems.begin(), systems.end(),
					[](auto& s) { return typeid(s) == typeid(T); }));
}

}
}
