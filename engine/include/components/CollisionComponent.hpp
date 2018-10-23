#pragma once

#include <scene/Entity.hpp>
#include <glm/glm.hpp>

#include <memory>

#include "Component.hpp"

namespace NAISE {
namespace Engine {

class CollisionComponent: public Component {
public:
	std::vector<glm::vec3> collisionPoints;
	std::vector<glm::vec3> collisionNormals;
	std::vector<EntityID> collisionEntities;
};

}
}
