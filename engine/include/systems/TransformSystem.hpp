#pragma once

#include "System.hpp"

#include <glm/glm.hpp>

namespace NAISE {
namespace Engine {

class TransformSystem: public System {
public:
	TransformSystem();

	void process(const EntityManager& em, microseconds deltaTime) override;

private:

	glm::mat4 evaluateNode(Entity& entity);

	Filter parentFilter;
	Filter transformFilter;
	Filter boundingBoxFilter;
};

}
}
