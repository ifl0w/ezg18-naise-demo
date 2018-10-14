#pragma once

#include "Shader.hpp"

namespace NAISE {
namespace Engine {

class SolidColorShader :
		public Shader {
public:
	SolidColorShader();

	void setColor(glm::vec3 color);
private:
	GLuint colorLocation;
};

}
}