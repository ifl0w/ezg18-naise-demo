#pragma once

#include <systems/input/InputMapper.hpp>
#include <unordered_map>
#include <typeindex>

#include "Component.hpp"

using namespace std;

namespace NAISE {
namespace Engine {

class InputComponent : public Component {
public:

	bool hasAction(Action a);

	Input& action(Action a);

	template <typename T>
	bool hasAction();

	template <typename T>
	Input& action();


	template<typename T>
	void add();

	unordered_map<Action, Input> mapping;
};

template<typename T>
void InputComponent::add() {
	mapping.emplace(pair(type_index(typeid(T)), Input()));
}

template<typename T>
bool InputComponent::hasAction() {
	return mapping.find(type_index(typeid(T))) != mapping.end();;
}

template<typename T>
Input& InputComponent::action() {
	return mapping.at(type_index(typeid(T)));
}

}
}
