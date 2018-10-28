#include <scene/EntityManager.hpp>
#include <Engine.hpp>

using namespace NAISE::Engine;

void EntityManager::addEntity(shared_ptr<Entity> entity) {
	// add to entity list
	entities.push_back(entity);
	// add to EntityID map
	entityMap.insert(pair(entity->id, entity.get()));

	// add to signatures
	for (auto& signature: signatures) {
		if (signature.second->match(*entity.get())) {
			signature.second->entities.push_back(entity.get());
		}
	}

	Engine::getEventManager().event<RuntimeEvents::EntityAdded>().emit(entity->id);
}

Entity* EntityManager::getEntity(EntityID id) {
	auto it = entityMap.find(id);

	if (it != entityMap.end()) {
		return it->second;
	}

	return nullptr;
}

void EntityManager::removeEntity(EntityID id) {
	Engine::getEventManager().event<RuntimeEvents::EntityRemoved>().emit(id);

	// remove from signatures
	for (auto& signature: signatures) {
		auto& s = signature.second;
		if (s->match(*entityMap[id])) {
			s->entities.erase(remove_if(s->entities.begin(), s->entities.end(), [&](auto ptr){
			  return ptr->id == id;
			}), s->entities.end());
		}
	}

	// remove from EntityID map
	entityMap.erase(id);

	// remove from entity list
	entities.erase(remove_if(entities.begin(), entities.end(), [&](auto& ptr){
	  return ptr->id == id;
	}), entities.end());
}

void EntityManager::filter(Filter filter, function<void(Entity&)> filterCallback) const {
	for (auto& e: entities) {
		if (filter.match(*e.get())) {
			filterCallback(*e.get());
		}
	}
}

void EntityManager::filter(Filter filter, function<void(Entity*)> filterCallback) const {
	for (auto& e: entities) {
		if (filter.match(*e.get())) {
			filterCallback(e.get());
		}
	}
}

void EntityManager::filter(Filter filter, function<void(vector<Entity*>)> filterCallback) const {
	vector<Entity*> result;

	for (auto& e: entities) {
		if (filter.match(*e.get())) {
			result.push_back(e.get());
		}
	}

	filterCallback(result);
}

void EntityManager::cleanup() {
	entities.clear();
	entityMap.clear();

	for (auto& s: signatures) {
		s.second->entities.clear();
	}
}

void EntityManager::updateSignatures(EntityID id) {
	auto it = entityMap.find(id);

	if (it == entityMap.end()) {
		return;
	}

	for (auto& s: signatures) {
		s.second->update(it->second);
	}
}

void EntityManager::addEntities(vector<shared_ptr<Entity>> entities) {
	for(auto& entity: entities) {
		addEntity(entity);
	}
}

void EntityManager::removeEntities(std::vector<std::shared_ptr<Entity>> entities) {
	for(auto& entity: entities) {
		removeEntity(entity->id);
	}
}

