#pragma once

#include "Entity.hpp"
#include "filter/Filter.hpp"
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

	void filter(Filter filter, function<void (Entity&)> filterCallback) const;
private:
	vector<shared_ptr<Entity>> entities;
};

}
}
