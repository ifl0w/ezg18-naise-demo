#include <systems/render-engine/materials/shaders/Shader.hpp>

#include <sstream>
#include <utility>

using namespace NAISE::Engine;

int64_t Shader::activeShader = -1;

std::vector<std::shared_ptr<LightData>> Shader::activeLights;

Shader::Shader(std::string vertexShaderFile, std::string fragmentShaderFile)
		: shaderID(createShaderProgram(std::move(vertexShaderFile), std::move(fragmentShaderFile))) {
	this->modelMatrixLocation = uniformLocation(this->shaderID, "modelMatrix");
}

Shader::~Shader() {
	glDeleteProgram((GLuint) shaderID);
}

void Shader::useShader() {
	Shader::activeShader = shaderID;
	glUseProgram((GLuint) this->shaderID);
}

void Shader::setModelMatrix(glm::tmat4x4<float> modelMatrix) {
	glUniformMatrix4fv(this->modelMatrixLocation, 1, false, glm::value_ptr(modelMatrix));
}
