#pragma once

#include <memory>

#include <BulletDynamics/Dynamics/btRigidBody.h>

#include "Component.hpp"

namespace NAISE {
namespace Engine {

class TagComponent: public Component {
public:
	TagComponent() = default;
	explicit TagComponent(std::string tag): tag(std::move(tag)) {};
	
	std::string tag;
};

}
}
