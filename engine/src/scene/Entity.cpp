#include <scene/Entity.hpp>
#include <Logger.hpp>
#include <Engine.hpp>

using namespace NAISE::Engine;

EntityID Entity::_lastID = 0;

void Entity::notifyEntityManager() {
	Engine::getEntityManager().updateSignatures(this->id);
}
