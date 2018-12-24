#pragma once

#include <vector>
#include <map>
#include <memory>

#include "ShaderUtils.hpp"

namespace NAISE {

namespace Engine {

class CameraComponent; // forward declaration to break cyclic dependency

}

namespace RenderCore {

class LightData;

class Shader {
public:
	Shader(std::string vertexShaderFile, std::string fragmentShaderFile);

	~Shader();

	const GLint shaderID;

	virtual void useShader();

	void setModelMatrix(glm::tmat4x4<float> modelMatrix);

	/* Static variables */
	static float brightness;
	static NAISE::Engine::CameraComponent& activeCamera;
	static int64_t activeShader;
//	static DirectionalLight& activeSun;
	static std::vector<std::shared_ptr<LightData>> activeLights;
private:
	GLint modelMatrixLocation = -1;
};

}
}