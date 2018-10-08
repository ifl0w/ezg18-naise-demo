#pragma once

#include <typeindex>
#include <any>

namespace NAISE {
namespace Engine {

using Payload = std::any;

using EventType = std::type_index;

template <typename T>
class Event {
public:
	const EventType eventType;
	const Payload payload;
};

}
}
