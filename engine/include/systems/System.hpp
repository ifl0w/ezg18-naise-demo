#pragma once

#include <scene/EntityManager.hpp>
#include "SystemsManager.hpp"
#include "Event.hpp"

#include <chrono>

using namespace std::chrono;

namespace NAISE {
namespace Engine {

class System {
	friend SystemsManager;

public:
	virtual void process(const EntityManager& em, microseconds deltaTime) = 0;

protected:
	SystemsManager* _systemsManager;

//	void notify(Event event) {};
};


}
}
