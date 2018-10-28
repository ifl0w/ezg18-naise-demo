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

  virtual void update(Entity* entity) { };
  vector<Entity*> entities;
};

template<typename ... ComponentTypes>
struct Signature : public SignatureBase {
  bool match(Entity& entity) override {
	  bool x = (entity.has<ComponentTypes>() && ...);
	  return x;
  }

  static bool test(Entity& entity) {
	  bool x = (entity.has<ComponentTypes>() && ...);
	  return x;
  }

  void update(Entity* entity) override {
	  auto it = find_if(entities.begin(), entities.end(), [=](auto e) { return e->id == entity->id; });

	  // not matching and contained (remove)
	  if (!match(*entity) && it != entities.end()) {
		  entities.erase(it);
	  }
	  // matching and not contained (insert)
	  if (match(*entity) && it == entities.end()) {
		  entities.push_back(entity);
	  }
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
	 * Add multiple entities at once. Forwards to addEntity(...).
	 * @param entities
	 */
	void addEntities(std::vector<std::shared_ptr<Entity>> entities);

	/**
	 * Remove an entity and its component by its ID.
	 *
	 * @param id
	 */
	void removeEntity(EntityID id);

	/**
	 * Remove multiple entities at once. Forwards to removeEntity(...).
	 * @param entities
	 */
	void removeEntities(std::vector<std::shared_ptr<Entity>> entities);

	/**
	 * Get the entity by its ID.
	 * Returns a pointer to the entity or null if no such entity exists.
	 *
	 * @param id
	 * @return
	 */
	Entity* getEntity(EntityID id);

	void filter(Filter filter, function<void(Entity&)> filterCallback) const;
	void filter(Filter filter, function<void(Entity*)> filterCallback) const;
	void filter(Filter filter, function<void(vector<Entity*>)> filterCallback) const;

	void cleanup();

	/**
	 * Update all signatures if the components of a given Entity changed.
	 *
	 * @param id
	 */
	void updateSignatures(EntityID id);

	template<typename T>
	void addSignature() {
		auto ptr = make_unique<T>();

		for (auto& e: entities) {
			ptr->update(e.get());
		}

		signatures.insert(pair(type_index(typeid(T)), std::move(ptr)));
	}

	template<typename T>
	T* getSignature() {
		T* tmp;

		try {
			tmp = static_cast<T*>(signatures.at(type_index(typeid(T))).get());
		} catch (const std::out_of_range& e) {
			throw std::invalid_argument("Signature not registered.");
		}

		return tmp;
	}

	template<typename T>
	vector<Entity*>& getEntities() {
		return getSignature<T>()->entities;
	}

private:
	vector<shared_ptr<Entity>> entities;
	unordered_map<EntityID, Entity*> entityMap;

	// Signatures keep track of entities and allow fast iteration.
	unordered_map<type_index, unique_ptr<SignatureBase>> signatures;

};

}
}
