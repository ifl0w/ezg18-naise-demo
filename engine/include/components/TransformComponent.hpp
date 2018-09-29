#pragma once

#include "Component.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

using namespace glm;

namespace NAISE {
namespace Engine {

class TransformComponent: public Component {
public:
	vec3 position;
	quat rotation;

	mat4 transform;
};

}
}
