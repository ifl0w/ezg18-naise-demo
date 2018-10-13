#pragma once

#include "Event.hpp"

#include <memory>

namespace NAISE {
namespace Engine {

class EventManager {
public:
	template <typename T>
	T& event();

private:
	std::unordered_map<EventType, std::unique_ptr<EventBase>> events;
};

template<typename T>
T& EventManager::event() {
	auto typeIdx = std::type_index(typeid(T));
	auto exists = events.count(typeIdx);

	if (!exists) {
		events.insert(std::pair(typeIdx, std::make_unique<T>()));
	}

	auto event = static_cast<T*>(events[typeIdx].get());
	return *event;
}

}
}
