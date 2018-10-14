#pragma once

#include <systems/render-engine/meshes/SkyboxMesh.hpp>
#include <systems/render-engine/textures/SkyboxTexture.hpp>
#include <systems/render-engine/shaders/SkyboxShader.hpp>

using namespace NAISE::RenderCore;

namespace NAISE {
namespace Engine {

class Skybox {
public:

	Skybox();
	Skybox(glm::vec3 backgroundColor);
	Skybox(const std::string& identifier, std::vector<std::string> paths);
	//Skybox(const glm::vec3 backgroundColor, const std::string& identifier, const std::vector<std::string> paths);

	~Skybox();

	void drawSkybox();

	void setBackgroundColor(glm::vec3 backgroundColor);
	void setModelMatrix(glm::mat4 modelMatrix);
	std::shared_ptr<Texture> getSkyboxTexture();


private:

	void initialize();
	void useSkybox() const;

	std::shared_ptr<Texture> skyboxTexture;
	SkyboxMesh skyboxMesh = SkyboxMesh(10, 10, 10);
	glm::vec3 backgroundColor = glm::vec3(1,1,1);
	bool useSkyboxTexture = false;

	GLint backgroundColorLocation = -1;
	GLint useSkyboxTextureLocation = -1;
	GLint skyboxTextureLocation = -1;
	uint32_t skyboxTextureUnit = 12;
	std::shared_ptr<Shader> skyboxShader;

	glm::mat4 modelMatrix = mat4(1);

};
}
}