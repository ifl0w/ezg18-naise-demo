#pragma once

#include <scene/EntityManager.hpp>

#include <chrono>

using namespace std::chrono;

namespace NAISE {
namespace Engine {

class System {
public:
	virtual void process(const EntityManager& em, microseconds deltaTime) = 0;
};

}
}
