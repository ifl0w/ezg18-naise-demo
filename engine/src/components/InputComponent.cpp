
#include <components/InputComponent.hpp>

#include "components/InputComponent.hpp"

using namespace NAISE::Engine;

bool InputComponent::hasAction(NAISE::Engine::Action a) {
	return mapping.find(a) != mapping.end();
}

Input& InputComponent::action(NAISE::Engine::Action a) {
	return mapping.at(a);
}
