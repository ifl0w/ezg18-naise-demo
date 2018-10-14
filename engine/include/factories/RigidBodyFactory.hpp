#pragma once

#include <components/RigidBodyComponent.hpp>
#include <glm/glm.hpp>

namespace NAISE {
namespace Engine {

class RigidBodyFactory {
public:
	static std::shared_ptr<RigidBodyComponent> createSphere(float radius = 1, float mass = 0.0, glm::vec3 position = glm::vec3(0), bool kinematic = false);
	static std::shared_ptr<RigidBodyComponent> createBox(float width, float height, float depth, float mass = 0.0, glm::vec3 position = glm::vec3(0), bool kinematic = false);
};

}
}
