#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>

#include "Event.hpp"
#include "scene/EntityManager.hpp"

namespace NAISE {
namespace Engine {

class System; // forward decl

class SystemsManager {
public:
	template<typename T, typename ... Args>
	void registerSystem(Args... args);

	template <typename T>
	T& getSystem();

	template<typename T>
	void removeSystem();

	template <typename T>
	T& registerEvent();

	template <typename T>
	T& event();

	void process(const EntityManager& em, std::chrono::microseconds deltaTime);

private:
	std::unordered_map<std::type_index, std::shared_ptr<System>> systems;
	std::unordered_map<EventType, std::unique_ptr<EventBase>> events;
};

template<typename T, typename... Args>
void SystemsManager::registerSystem(Args... args) {
	auto s = std::make_shared<T>(args ...);
	systems.insert(pair(std::type_index(typeid(T)), s));
	s->setManager(this);
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

template<typename T>
T& SystemsManager::registerEvent() {
	auto typeIdx = std::type_index(typeid(T));
	auto exists = events.count(typeIdx);

	if (!exists) {
		events.insert(std::pair(typeIdx, make_unique<T>()));
	}

	auto event = static_cast<T*>(events[typeIdx].get());
	return *event;
}

template<typename T>
T& SystemsManager::event() {
	auto typeIdx = std::type_index(typeid(T));
	auto exists = events.count(typeIdx);

	if (!exists) {
		events.insert(std::pair(typeIdx, make_unique<T>()));
	}

	auto event = static_cast<T*>(events[typeIdx].get());
	return *event;
}


}
}
