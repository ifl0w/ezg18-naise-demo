#pragma once

#include <scene/EntityManager.hpp>

#include <chrono>

using namespace std::chrono;

namespace NAISE {
namespace Engine {

class System {

public:
	~System() { }

	virtual void process(microseconds deltaTime) = 0;
};


}
}
