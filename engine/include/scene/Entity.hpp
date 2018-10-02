#pragma once

#include <components/Component.hpp>

#include <unordered_map>
#include <memory>
#include <typeindex>
#include <spdlog/spdlog.h>

using namespace std;

namespace NAISE {
namespace Engine {

using EntityID = uint64_t;

class Entity {
public:
	explicit Entity(): id(Entity::_lastID++) { };

	const EntityID id;

	/**
	 * Add a component to the entity. The entity will take ownership of the given component.
	 * An entity can only contain a single component of a given type.
	 *
	 * @param component
	 */
	template <class T>
	void add(shared_ptr<T> component);

	template <class T, typename... Args>
	void add(Args&& ... args);

	template <class T>
	T& component();

	template <class T>
	void removeComponent();

private:
	friend class Filter;
	unordered_map<type_index, shared_ptr<Component>> components;

	static EntityID _lastID;
};

template<class T>
void Entity::add(shared_ptr<T> component) {
	components[type_index(typeid(T))] = component;
}

template<class T, typename... Args>
void Entity::add(Args&& ... args) {
	components[type_index(typeid(T))] = make_shared<T>();
}

template<class T>
T& Entity::component() {
	try {
		auto tmp = components.at(type_index(typeid(T)));
	} catch (std::out_of_range& e) {
		throw std::invalid_argument("Entity does not contain the requested component.");
	}
	auto tmp = static_cast<T*>(components.at(type_index(typeid(T))).get());
	return *tmp;
}

template<class T>
void Entity::removeComponent() {
	components.erase(type_index(typeid(T)));
}

}
}
