#include <scene/EntityManager.hpp>

using namespace NAISE::Engine;

void EntityManager::addEntity(shared_ptr<Entity> entity) {
	entities[entity->id] = (entity);
}

void EntityManager::addSystem(shared_ptr<System> system) {
	systems.push_back(system);
}
