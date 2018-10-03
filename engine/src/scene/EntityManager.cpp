#include <scene/EntityManager.hpp>

using namespace NAISE::Engine;

void EntityManager::addEntity(shared_ptr<Entity> entity) {
	entities.push_back(entity);
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

