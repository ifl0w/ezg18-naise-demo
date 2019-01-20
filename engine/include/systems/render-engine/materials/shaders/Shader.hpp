#pragma once

#include <vector>
#include <map>
#include <memory>

#include "ShaderUtils.hpp"

namespace NAISE {

namespace RenderCore {

class LightData;

class Shader {
public:
	Shader(std::string vertexShaderFile, std::string fragmentShaderFile);

	~Shader();

	//const GLint shaderID;
	GLint shaderID;

	virtual void useShader();

	virtual void initUniformLocations();

	void recompile();

	void setModelMatrix(glm::tmat4x4<float> modelMatrix);

	/* Static variables */
	static float brightness;
	static int64_t activeShader;
//	static DirectionalLight& activeSun;
	static std::vector<std::shared_ptr<LightData>> activeLights;
private:
	GLint modelMatrixLocation = -1;

	std::string m_vertexShaderFile;
	std::string m_fragmentShaderFile;
};

}
}