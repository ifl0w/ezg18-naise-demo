#pragma once

#include <stdint.h>

#include <typeindex>
#include <functional>

namespace NAISE {
namespace Engine {

using EventID = uint32_t;
using EventType = std::type_index;

struct EventBase {};

template <typename ... Args>
class Event: public EventBase {
public:
	EventID subscribe(std::function<void (Args ... args)> fun) {
		subscriptions.insert(std::pair(_lastID++, fun));
		return _lastID;
	};

	void unsubscribe(EventID id) {
		subscriptions.erase(id);
	};

	void emit(Args ... args) {
		for (auto& sub: subscriptions) {
			sub.second(args ...);
		}
	};

private:
	std::unordered_map<EventID, std::function<void (Args ... args)>> subscriptions;
	EventID _lastID = 0;
};


}
}
