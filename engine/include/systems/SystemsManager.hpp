#pragma once

#include <memory>
#include <vector>

//#include "System.hpp"

using namespace std;

//class System;

namespace NAISE {
namespace Engine {

class SystemsManager {

//	template<typename T, typename ... Args>
//	void registerSystem(Args... args);
//
//	template<typename T>
//	void removeSystem();

//	template<typename T>
//	void subscribe(std::function<void (Payload payload)> callback);
//
//	template<typename T, typename P>
//	void broadcast(P payload);
//
//	template<typename T, typename S, typename P>
//	void wisper(P payload);

private:
//	vector<shared_ptr<System>> systems;

//	unordered_map<EventType, vector<std::function<void (Payload payload)>>> subscriptions;
};
//
//template<typename T, typename... Args>
//void SystemsManager::registerSystem(Args... args) {
//	auto& s = make_shared<T>(args ...);
//	systems.push_back(s);
//	s.systemsManager = this;
//}
//
//template<typename T>
//void SystemsManager::removeSystem() {
//	erase(remove_if(systems.begin(), systems.end(),
//			[](auto& s) { return typeid(s) == typeid(T); }));
//}

//template<typename T, typename P>
//void SystemsManager::broadcast(P payload) {
//	for (auto& s: systems) {
//		s->notify(payload);
//	}
//}
//
//template<typename T, typename S, typename P>
//void SystemsManager::wisper(Event payload) {
//	for (auto& s: systems) {
//		if (typeid(s) == typeid(S)) {
//			s->notify(payload);
//		}
//	}
//}


}
}
