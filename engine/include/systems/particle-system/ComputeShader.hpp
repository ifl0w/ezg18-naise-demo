#pragma once

#include <string>
#include <glbinding/gl/gl.h>

namespace NAISE {
namespace Engine {

class ComputeShader {
public:
	explicit ComputeShader(std::string computeShaderFile);

	~ComputeShader();

	const gl::GLint shaderID;

	void useShader() const;
	void compute(uint64_t groups) const;
};

}
}

