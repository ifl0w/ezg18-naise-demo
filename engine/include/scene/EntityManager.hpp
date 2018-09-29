#pragma once

#include "Entity.hpp"
#include "systems/System.hpp"

#include <vector>
#include <memory>
#include <map>

using namespace std;

namespace NAISE {
namespace Engine {

class EntityManager {
public:
	/**
	 * Add the entity to the EntityManager.
	 * The EntityManager takes ownership of the given entity.
	 *
	 * @param entity
	 */
	void addEntity(shared_ptr<Entity> entity);

	/**
	 * @param system
	 */
	void addSystem(shared_ptr<System> system);
private:
	vector<shared_ptr<System>> systems;
	map<uint64_t, shared_ptr<Entity>> entities;
};

}
}
