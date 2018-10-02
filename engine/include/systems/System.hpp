#pragma once

namespace NAISE {
namespace Engine {

class EntityManager;

class System {
public:
	virtual void process(const EntityManager& em) = 0;
};

}
}
