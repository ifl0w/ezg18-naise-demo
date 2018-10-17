#pragma once

#include "Entity.hpp"
#include "filter/Filter.hpp"

#include <vector>
#include <memory>
#include <map>

using namespace std;

namespace NAISE {
namespace Engine {

class EntityManager {
public:
	~EntityManager() {
		entities.clear();
	}

	/**
	 * Add the entity to the EntityManager.
	 * The EntityManager takes ownership of the given entity.
	 *
	 * @param entity
	 */
	void addEntity(shared_ptr<Entity> entity);

	/**
	 * Remove an entity and its component by its ID.
	 *
	 * @param id
	 */
	void removeEntity(EntityID id);

	/**
	 * Get the entity by its ID.
	 * Returns a pointer to the entity or null if no such entity exists.
	 *
	 * @param id
	 * @return
	 */
	Entity* getEntity(EntityID id);

	void filter(Filter filter, function<void (Entity&)> filterCallback) const;
	void filter(Filter filter, function<void (Entity*)> filterCallback) const;
	void filter(Filter filter, function<void (vector<Entity*>)> filterCallback) const;

	void cleanup();

private:
	vector<shared_ptr<Entity>> entities;
};

}
}
