#pragma once

#include <components/Component.hpp>
#include <aspects/Aspect.hpp>

#include <unordered_map>
#include <memory>
#include <typeindex>
#include <spdlog/spdlog.h>

using namespace std;

namespace NAISE {
namespace Engine {

class Entity {
public:
	explicit Entity(uint64_t id): id(id) { };

	const uint64_t id;

	/**
	 * Add a component to the entity. The entity will take ownership of the given component.
	 * An entity can only contain a single component of a given type.
	 *
	 * @param component
	 */
	template <class T>
	void addComponent(shared_ptr<T> component);

	template <class T, typename... Args>
	void addComponent(Args&&... args);

	template <class T>
	T& component();

	template <class T>
	void removeComponent();

	/**
	 * Match against a given aspect.
	 *
	 * @param aspect
	 * @return
	 */
	virtual bool match(shared_ptr<Aspect>& aspect);

private:
	unordered_map<type_index, shared_ptr<Component>> components;
};

template<class T>
void Entity::addComponent(shared_ptr<T> component) {
	components[type_index(typeid(T))] = component;
}

template<class T, typename... Args>
void Entity::addComponent(Args&&... args) {
	components[type_index(typeid(T))] = make_shared<T>();
}

template<class T>
T& Entity::component() {
	return components[type_index(typeid(T))].get();
}

template<class T>
void Entity::removeComponent() {
	components.erase(type_index(typeid(T)));
}

}
}
