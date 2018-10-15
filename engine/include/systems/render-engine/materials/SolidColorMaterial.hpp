#pragma once

#include "Material.hpp"

using namespace NAISE::RenderCore;

namespace NAISE {
namespace Engine {

class SolidColorMaterial : public Material {
public:
	SolidColorMaterial();

	explicit SolidColorMaterial(glm::vec3 color);

	void useMaterial() const override;

	glm::vec3 color;
private:
	GLint colorLocation;
};

}
}
