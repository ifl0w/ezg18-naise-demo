#pragma once

#include "Component.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

using namespace glm;

namespace NAISE {
namespace Engine {

class TransformSystem;

class TransformComponent: public Component {
	friend TransformSystem;
public:
	// these values are local properties of the entity
	vec3 position = vec3(0);
	quat rotation = angleAxis(radians<float>(0), vec3(0,1,0));
	vec3 scale = vec3(1);

	// global properties
	vec3 globalPosition = vec3(0);
	quat globalRotation = angleAxis(radians<float>(0), vec3(0,1,0));
	vec3 globalScale = vec3(1);

	mat4 getModelMatrix() const {
		return globalTransform;
	}

	void setModelMatrix(mat4 modelMatrix){
		globalTransform = modelMatrix;
	}

	mat4 getLocalTransform() const {
		return localTransform;
	}

	mat4 getParentTransform() const {
		return parentTransform;
	}

private:
	void calculateLocalModelMatrix() {
		glm::mat4 model = glm::dmat4(1);

		// reversed because glm operations do: "original * modifier"
		model = glm::translate(model, glm::vec3(position));

		mat4 rotationMatrix = mat4_cast(rotation);
		model = model * rotationMatrix;

		model = glm::scale(model, glm::vec3(scale));

		localTransform = model;

		// global values default to local values
		globalTransform = model;
		globalPosition = position;
		globalRotation = rotation;
		globalScale = scale;
	}

	void updateGlobalValues() {
		glm::vec3 skew; // unused
		glm::vec4 perspective; // unused
		glm::decompose(globalTransform, globalScale, globalRotation, globalPosition, skew, perspective);
	}

	mat4 localTransform = mat4(1);
	mat4 parentTransform = mat4(1);
	mat4 globalTransform = mat4(1);
};

}
}
