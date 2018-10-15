#include "filter/Filter.hpp"

using namespace NAISE::Engine;

bool Filter::match(const Entity& entity) {
	for(const type_index& r:_required) {
		if (entity.components.find(r) == entity.components.end()) {
			return false;
		}
	}

	return true;
}