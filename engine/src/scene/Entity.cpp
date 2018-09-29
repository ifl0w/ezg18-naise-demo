#include <scene/Entity.hpp>

#include <spdlog/spdlog.h>

using namespace NAISE::Engine;

bool Entity::match(shared_ptr<Aspect>& aspect) {
	return false;
}

