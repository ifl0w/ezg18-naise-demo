#include <systems/particle-system/ComputeShader.hpp>
#include <systems/render-engine/materials/shaders/Shader.hpp>

#include <systems/render-engine/materials/shaders/ShaderUtils.hpp>

using namespace NAISE::Engine;

ComputeShader::ComputeShader(std::string computeShaderFile)
		: shaderID(createComputeShader(std::move(computeShaderFile))) {

}

ComputeShader::~ComputeShader() {
	glDeleteProgram(shaderID);
}

void ComputeShader::useShader()const {
	Shader::activeShader = shaderID;
	glUseProgram(this->shaderID);
}

void ComputeShader::compute(uint64_t groups) const {
	glDispatchCompute(groups, 1, 1);
}
