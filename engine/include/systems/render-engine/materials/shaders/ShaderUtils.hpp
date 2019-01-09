#pragma once

#include <glbinding/gl/gl.h>
#include <glm/gtc/type_ptr.hpp>

using namespace gl;

namespace NAISE {
namespace Engine {

GLuint createComputeShader(std::string singleShaderFile);
GLuint createShaderProgram(std::string vertexShaderFile, std::string fragmentShaderFile);
GLuint reloadShaderProgram(std::string vertexShaderFile, std::string fragmentShaderFile);

GLint uniformLocation(GLint shader, std::string name);

}
}