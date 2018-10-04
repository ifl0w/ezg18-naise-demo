#pragma once

#include "Component.hpp"

#include <vector>
#include <memory>
#include <typeindex>
#include <glm/glm.hpp>
#include <glbinding/gl/gl.h>

#include <systems/render-engine/lights/Light.hpp>

using namespace glm;
using namespace gl;
using namespace std;
using namespace NAISE::RenderCore;

namespace NAISE {
namespace Engine {

class LightComponent: public Component {

using LightType = type_index;

public:
	LightComponent(): light(make_shared<Light>()), type(type_index(typeid(Light))) {};

	LightType type;
	shared_ptr<Light> light;

	template<typename T>
	bool isType() {
		return type_index(typeid(T)) == type;
	}

	// AABB aabb;
};

}
}
