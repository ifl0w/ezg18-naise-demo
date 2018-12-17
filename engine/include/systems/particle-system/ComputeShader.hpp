#pragma once

#include <string>
#include <glbinding/gl/gl.h>
#include <glm/glm.hpp>

namespace NAISE {
namespace Engine {

class ComputeShader {
public:
	explicit ComputeShader(std::string computeShaderFile);

	~ComputeShader();

	const gl::GLuint shaderID;

	void useShader() const;
	void compute(uint64_t groups) const;
	void compute(glm::ivec3 groups) const;
};

}
}

