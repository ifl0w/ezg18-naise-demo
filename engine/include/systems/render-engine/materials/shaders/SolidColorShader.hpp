#pragma once

#include "Shader.hpp"

namespace NAISE {
namespace Engine {

class SolidColorShader :
		public RenderCore::Shader {
public:
	SolidColorShader();

	void setColor(glm::vec3 color);
private:
	GLuint colorLocation;
};

}
}