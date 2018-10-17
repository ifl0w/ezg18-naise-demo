#pragma once

#include "Component.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

using namespace glm;

namespace NAISE {
namespace Engine {

class TransformSystem;

class TransformComponent: public Component {
	friend TransformSystem;
public:
	vec3 position;
	vec3 scale = vec3(1);
	quat rotation = angleAxis(radians<float>(0), vec3(0,1,0));

	mat4 getModelMatrix() const {
		return transform;
	}

private:
	void calculateModelMatrix() {
		glm::mat4 model = glm::dmat4(1);

		// reversed because glm operations do: "original * modifier"
		model = glm::translate(model, glm::vec3(position));

		mat4 rotationMatrix = mat4_cast(rotation);
		model = model * rotationMatrix;

		model = glm::scale(model, glm::vec3(scale));

		transform = model;
	}

	mat4 transform;
};

}
}
