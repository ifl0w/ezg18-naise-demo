#include <systems/render-engine/materials/GouraudMaterial.hpp>
#include <systems/render-engine/materials/shaders/GouraudShader.hpp>
#include <Resources.hpp>

using namespace NAISE::RenderCore;

GouraudMaterial::GouraudMaterial()
		: GouraudMaterial(vec3(0.7f)) {
}

GouraudMaterial::GouraudMaterial(vec3 diffuse)
		: GouraudMaterial(diffuse, diffuse * 0.3f, diffuse * 0.2f, 1.0f) {

}

GouraudMaterial::GouraudMaterial(vec3 diffuse, vec3 specular, vec3 ambient, float alpha)
		: diffuse(diffuse),
		  specular(specular),
		  ambient(ambient),
		  shininess(alpha) {
	shader = Resources::getShader<GouraudShader>();

	this->materialDiffLocation = uniformLocation(shader->shaderID, "material.diffuse");
	this->materialSpecLocation = uniformLocation(shader->shaderID, "material.specular");
	this->materialAmbiLocation = uniformLocation(shader->shaderID, "material.ambient");
	this->materialShinLocation = uniformLocation(shader->shaderID, "material.shininess");

	this->colorTextureLocation = uniformLocation(shader->shaderID, "colorTexture");
	this->useColorTextureLocation = uniformLocation(shader->shaderID, "useColorTexture");

	this->specularTextureLocation = uniformLocation(shader->shaderID, "specularTexture");
	this->useSpecularTextureLocation = uniformLocation(shader->shaderID, "useSpecularTexture");
}

GouraudMaterial::GouraudMaterial(vec3 color, float diffuseFactor, float specularFactor, float ambientFactor,
								 float alpha)
		: GouraudMaterial((color * diffuseFactor), (color * specularFactor), (color * ambientFactor), alpha) {
}

void GouraudMaterial::useMaterial() const {
	glUniform3fv(this->materialDiffLocation, 1, glm::value_ptr(diffuse));
	glUniform3fv(this->materialSpecLocation, 1, glm::value_ptr(specular));
	glUniform3fv(this->materialAmbiLocation, 1, glm::value_ptr(ambient));
	glUniform1f(this->materialShinLocation, shininess);

//	if (diffuseTexture) {
//		glUniform1i(useColorTextureLocation, true);
//		glUniform1i(colorTextureLocation, colorTextureUnit);
//		diffuseTexture->useTexture(colorTextureUnit);
//	} else {
//		glUniform1i(useColorTextureLocation, false);
//	}
//
//	if (specularTexture) {
//		glUniform1i(useSpecularTextureLocation, true);
//		glUniform1i(specularTextureLocation, specularTextureUnit);
//		specularTexture->useTexture(specularTextureUnit);
//	} else {
//		glUniform1i(useSpecularTextureLocation, false);
//	}
}
