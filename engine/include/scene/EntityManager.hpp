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

	void filter(Filter filter, function<void (Entity&)> filterCallback) const;
	void filter(Filter filter, function<void (Entity*)> filterCallback) const;
	void filter(Filter filter, function<void (vector<Entity*>)> filterCallback) const;
private:
	vector<shared_ptr<Entity>> entities;
};

}
}
