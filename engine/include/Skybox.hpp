#pragma once

#include <systems/render-engine/meshes/SkyboxMesh.hpp>
#include <systems/render-engine/textures/SkyboxTexture.hpp>
#include <systems/render-engine/shaders/SkyboxShader.hpp>

using namespace NAISE::RenderCore;

namespace NAISE {
namespace Engine {

class Skybox {
public:

	/**
	 * The default constructor constructs a skybox with R:1 G:1 B:1 as default background color.
	 * No textures are needed
	 */
	Skybox();

	/**
	 * Constructs a skybox with the given background color.
	 * No textures are needed
	 * @param backgroundColor
	 */
	Skybox(glm::vec3 backgroundColor);

	/**
	 * Constructs a skybox with the give textures.
	 * @param identifier is used to retrieve a texture, if it already exist
	 * @param paths of the 6 skybox textures
	 */
	Skybox(const std::string &identifier, std::vector<std::string> paths);

	/**
	 * Constructs a skybox with the given background color and textures.
	 * @param backgroundColor
	 * @param identifier
	 * @param paths
	 */
	Skybox(const glm::vec3 backgroundColor, const std::string &identifier, const std::vector<std::string> paths);

	~Skybox();

	void drawSkybox();

	/**
	 * Sets the background color. THis color will be multiplied with the texture - if available.
	 * Otherwise it will be a solid background color.
	 *
	 * @param backgroundColor
	 */
	void setBackgroundColor(glm::vec3 backgroundColor);

	void setSkyboxTexture(const std::string &identifier, std::vector<std::string> paths);

	void setModelMatrix(glm::mat4 modelMatrix);

	std::shared_ptr<Texture> getSkyboxTexture();

	void applyToShader(shared_ptr<Shader> shared_ptr);

	/**
	 * The brighness factor will be multiplied with the color.
	 * Used to adjust brighnest for HDR.
	 */
	float brightness = 1000;

private:

	void initialize();

	void useSkybox() const;

	SkyboxMesh skyboxMesh = SkyboxMesh(10, 10, 10);
	glm::mat4 modelMatrix = mat4(1);
	glm::vec3 backgroundColor = glm::vec3(1, 1, 1);
	std::shared_ptr<Texture> skyboxTexture;

	bool useSkyboxTexture = false;
	GLint backgroundColorLocation = -1;
	GLint useSkyboxTextureLocation = -1;
	GLint skyboxTextureLocation = -1;
	GLint brightnessLocation = -1;
	uint32_t skyboxTextureUnit = 12;

	std::shared_ptr<Shader> skyboxShader;

};
}
}