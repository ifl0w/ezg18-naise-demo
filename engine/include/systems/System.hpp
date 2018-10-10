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
	~System() {
		if (_systemsManager != nullptr) {
		}
	}

	virtual void process(const EntityManager& em, microseconds deltaTime) = 0;

protected:
	SystemsManager* _systemsManager = nullptr;

	void setManager(SystemsManager* systemsManager) {
		_systemsManager = systemsManager;
		eventSetup();
	};

	virtual void eventSetup() {};
};


}
}
