#pragma once

#include <scene/Entity.hpp>
#include <systems/Event.hpp>
#include <Logger.hpp>

namespace NAISE {
namespace Engine {

using SignatureID = type_index;

struct SignatureBase {
  virtual bool match(Entity& entity) { return false; };

  virtual void add(Entity* entity) { };
  virtual void remove(Entity* entity) { };
  virtual void update(Entity* entity) { };

  vector<Entity*> entities;
};

template<typename ... ComponentTypes>
struct Signature : public SignatureBase {

  bool match(Entity& entity) override {
	  bool x = (entity.has<ComponentTypes>() && ...);
	  return x;
  }

  static bool test(Entity& entity) {
	  bool x = (entity.has<ComponentTypes>() && ...);
	  return x;
  }

  void update(Entity* entity) override;

  void add(Entity* entity) override;
  void remove(Entity* entity) override;

  virtual SignatureID id() {
	  return type_index(typeid(this)); };
};

template<typename... ComponentTypes>
void Signature<ComponentTypes...>::remove(Entity* entity) {
	auto it = find(entities.begin(), entities.end(), entity);

	if (it != entities.end()) {
		entities.erase(it);
	}
}

template<typename... ComponentTypes>
void Signature<ComponentTypes...>::update(Entity* entity) {
	auto it = find_if(entities.begin(), entities.end(), [=](auto e) { return e->id == entity->id; });

	// not matching and contained (remove)
	if (!match(*entity) && it != entities.end()) {
		entities.erase(it);
	}
	// matching and not contained (insert)
	if (match(*entity) && it == entities.end()) {
		entities.push_back(entity);
	}
}

template<typename... ComponentTypes>
void Signature<ComponentTypes...>::add(Entity* entity) {
	entities.push_back(entity);
}

}
}