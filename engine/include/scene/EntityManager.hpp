#pragma once

#include "Entity.hpp"
#include "filter/Filter.hpp"

#include <vector>
#include <memory>
#include <unordered_map>
#include <map>

using namespace std;

namespace NAISE {
namespace Engine {

struct SignatureBase {
  virtual bool match(Entity& entity) { return false; };
  vector<Entity*> entities;
};

template <typename ... ComponentTypes>
struct Signature: public SignatureBase {
  bool match(Entity& entity) override {
	bool x = (entity.has<ComponentTypes>() && ...);
  	return x;
  }
};

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

	template <typename T>
	void addSignature() {
		signatures.insert(pair(type_index(typeid(T)), make_unique<T>()));
	}

	template <typename T>
	T* getSignature() {
		return static_cast<T*>(signatures[type_index(typeid(T))].get());
	}

private:
	vector<shared_ptr<Entity>> entities;
	unordered_map<EntityID, Entity*> entityMap;

	// Signatures keep track of entities and allow fast iteration.
	unordered_map<type_index, unique_ptr<SignatureBase>> signatures;

};

}
}
