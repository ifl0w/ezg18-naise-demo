#pragma once

#include "../materials/shaders/Shader.hpp" //TODO make skyboxShader a materialShader???

namespace NAISE {
namespace Engine {

class SkyboxShader :
		public Shader {
public:
	SkyboxShader(): Shader("engine/shaders/skybox/skybox.vert", "engine/shaders/skybox/skybox.frag") {
		//this->backgroundColorLocation = uniformLocation(this->shaderID, "backgroundColor");
	//	this->useSkyboxTextureLocation = uniformLocation(this->shaderID, "useSkyboxTexture");
		//this->skyboxTextureLocation = uniformLocation(this->shaderID, "skyboxTexture");
	};

/*	void setBackgroundColor(glm::vec3 backgroundColor) {
		glUniform3fv(this->backgroundColorLocation, 1, glm::value_ptr(backgroundColor));
	}*/

private:
//	GLint backgroundColorLocation = -1;
	//GLint useSkyboxTextureLocation = -1;
//	GLint skyboxTextureLocation = -1;
};

}
}


