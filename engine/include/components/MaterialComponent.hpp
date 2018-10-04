#pragma once

#include "Component.hpp"

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glbinding/gl/gl.h>

#include <systems/render-engine/materials/Material.hpp>

using namespace glm;
using namespace gl;
using namespace std;
using namespace NAISE::RenderCore;

namespace NAISE {
namespace Engine {

class MaterialComponent: public Component {
public:
	MaterialComponent() = default;
	MaterialComponent(shared_ptr<Material> material): material(material) {};

	shared_ptr<Material> material;
};

}
}
