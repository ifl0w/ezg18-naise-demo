#include <systems/render-engine/materials/shaders/Shader.hpp>
#include <systems/render-engine/lights/Light.hpp>

#include <sstream>
#include <utility>
#include <iostream>

using namespace NAISE::RenderCore;
using namespace NAISE::Engine;

int64_t Shader::activeShader = -1;

std::vector<std::shared_ptr<LightData>> Shader::activeLights;

Shader::Shader(std::string vertexShaderFile, std::string fragmentShaderFile)
		: m_vertexShaderFile(std::move(vertexShaderFile)), m_fragmentShaderFile(std::move(fragmentShaderFile)){
	shaderID = createShaderProgram(m_vertexShaderFile, m_fragmentShaderFile);
	initUniformLocations();
    //this->modelMatrixLocation = uniformLocation(this->shaderID, "modelMatrix");
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

void Shader::initUniformLocations(){
    this->modelMatrixLocation = uniformLocation(this->shaderID, "modelMatrix");
}

void Shader::recompile(){

	try {
	glEnable(GL_DEBUG_OUTPUT);
	glDeleteProgram((GLuint) shaderID);
   	shaderID = createShaderProgram(m_vertexShaderFile, m_fragmentShaderFile);
    initUniformLocations();
	} catch(std::runtime_error& e) {
		std::cout<< e.what()<<std::endl;
	}
}
