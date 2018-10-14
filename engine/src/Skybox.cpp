
#include <glm/glm.hpp>
#include <Skybox.hpp>
#include <Resources.hpp>

using namespace NAISE::Engine;

Skybox::Skybox(){
	initialize();
}

Skybox::Skybox(glm::vec3 backgroundColor)
	: backgroundColor(backgroundColor){
	initialize();
}

Skybox::Skybox(const std::string& identifier, std::vector<std::string> paths)
//		: skyboxTexture(Resources::loadSkyboxTexture(identifier, paths)){
{
	skyboxTexture = Resources::loadSkyboxTexture(identifier, paths);

	//SkyboxTexture t = SkyboxTexture(Resources::loadSkyboxTexture(identifier, paths));
	useSkyboxTexture = true;
	initialize();
}

Skybox::~Skybox() {

}

void Skybox::initialize() {
	skyboxShader = Resources::getShader<SkyboxShader>();
	this->backgroundColorLocation = uniformLocation(skyboxShader->shaderID, "backgroundColor");
	this->useSkyboxTextureLocation = uniformLocation(skyboxShader->shaderID, "useSkyboxTexture");
	this->skyboxTextureLocation = uniformLocation(skyboxShader->shaderID, "skyboxTexture");
}

void Skybox::drawSkybox() {

	if (skyboxShader->shaderID != Shader::activeShader) {
		skyboxShader->useShader();
	}

	skyboxShader->setModelMatrix(this->modelMatrix);
	useSkybox();
//	skyboxShader->setBackgroundColor(this->backgroundColor);

	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);

	glBindVertexArray(skyboxMesh.vao);

	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(skyboxMesh.indices.size()), GL_UNSIGNED_INT, 0);

	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);

}

void Skybox::setModelMatrix(glm::mat4 modelMatrix) {
	this->modelMatrix = modelMatrix;
}

void Skybox::setBackgroundColor(glm::vec3 backgroundColor) {
	this->backgroundColor = backgroundColor;
}

void Skybox::useSkybox() const{
	if(useSkyboxTexture){
		glUniform1i(skyboxTextureLocation, skyboxTextureUnit);
		skyboxTexture->useTexture(skyboxTextureUnit);
	}
	glUniform3fv(this->backgroundColorLocation, 1, glm::value_ptr(backgroundColor));
	glUniform1i(this->useSkyboxTextureLocation, useSkyboxTexture);
	glActiveTexture(GL_TEXTURE0);
}

std::shared_ptr<Texture> Skybox::getSkyboxTexture() {
	return skyboxTexture;
}
