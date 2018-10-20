#pragma once

#include "System.hpp"

#include <components/TransformComponent.hpp>
#include <components/ParentComponent.hpp>
#include <components/AABBComponent.hpp>

#include <glm/glm.hpp>

namespace NAISE {
namespace Engine {

struct TransformSignature: public Signature<TransformComponent> {};
struct ParentSignature: public Signature<ParentComponent> {};
struct AABBSignature: public Signature<TransformComponent, AABBComponent> {};

class TransformSystem: public System {
public:
	TransformSystem();

	void process(const EntityManager& em, microseconds deltaTime) override;

private:

	glm::mat4 evaluateNode(Entity& entity);
};

}
}
