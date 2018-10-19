#pragma once

#include "Component.hpp"
#include "scene/Entity.hpp"

#include <glm/glm.hpp>

namespace NAISE {
namespace Engine {

class TransformSystem;

class ParentComponent : public Component {
	friend TransformSystem;
public:

	ParentComponent() = default;
	explicit ParentComponent(EntityID parent): parent(parent) {};

	EntityID parent;

private:
	bool evaluated = false;
	glm::mat4 cachedParentTransform = glm::mat4(1);
};

}
}
