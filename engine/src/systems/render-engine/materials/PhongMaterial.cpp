#include <systems/render-engine/materials/PhongMaterial.hpp>
#include <systems/render-engine/materials/shaders/PhongShader.hpp>
#include <Resources.hpp>

using namespace NAISE::RenderCore;

bool PhongMaterial::displayNormalTexture = true;
bool PhongMaterial::displayGlowTexture = true;

PhongMaterial::PhongMaterial()
		: PhongMaterial(vec3(0.7f)) {
}

PhongMaterial::PhongMaterial(vec3 diffuse)
		: PhongMaterial(diffuse, diffuse * 0.3f, diffuse * 0.2f, 1.0f) {
}

PhongMaterial::PhongMaterial(vec3 diffuse, vec3 specular, vec3 ambient, float alpha)
		: diffuse(diffuse),
		  specular(specular),
		  ambient(ambient),
		  shininess(alpha) {
	shader = Resources::getShader<PhongShader>();

	this->materialDiffLocation = uniformLocation(shader->shaderID, "material.diffuse");
	this->materialSpecLocation = uniformLocation(shader->shaderID, "material.specular");
	this->materialAmbiLocation = uniformLocation(shader->shaderID, "material.ambient");
	this->materialShinLocation = uniformLocation(shader->shaderID, "material.shininess");
	this->materialGlowLocation = uniformLocation(shader->shaderID, "material.glow");

	this->colorTextureLocation = uniformLocation(shader->shaderID, "colorTexture");
	this->useColorTextureLocation = uniformLocation(shader->shaderID, "useColorTexture");

	this->specularTextureLocation = uniformLocation(shader->shaderID, "specularTexture");
	this->useSpecularTextureLocation = uniformLocation(shader->shaderID, "useSpecularTexture");

	this->normalTextureLocation = uniformLocation(shader->shaderID, "normalTexture");
	this->useNormalTextureLocation = uniformLocation(shader->shaderID, "useNormalTexture");

	this->glowTextureLocation = uniformLocation(shader->shaderID, "glowTexture");
	this->useGlowTextureLocation = uniformLocation(shader->shaderID, "useGlowTexture");

	this->useWatermeshAnimationLocation = uniformLocation(shader->shaderID, "useWatermeshAnimation");
}

PhongMaterial::PhongMaterial(vec3 color, float diffuseFactor, float specularFactor, float ambientFactor, float alpha)
		: PhongMaterial((color * diffuseFactor), (color * specularFactor), (color * ambientFactor), alpha) {
}

void PhongMaterial::useMaterial() const {
	glUniform3fv(this->materialDiffLocation, 1, glm::value_ptr(diffuse));
	glUniform3fv(this->materialSpecLocation, 1, glm::value_ptr(specular));
	glUniform3fv(this->materialAmbiLocation, 1, glm::value_ptr(ambient));
	glUniform3fv(this->materialGlowLocation, 1, glm::value_ptr(glow));
	glUniform1f(this->materialShinLocation, shininess);
	glUniform1i(this->useWatermeshAnimationLocation, false);
//
//	if (diffuseTexture) {
//		glUniform1i(useColorTextureLocation, true);
//		glUniform1i(colorTextureLocation, colorTextureUnit);
//		diffuseTexture->useTexture(colorTextureUnit);
//	} else {
		glUniform1i(useColorTextureLocation, false);
//	}

//	if (normalTexture && displayNormalTexture) {
//		glUniform1i(useNormalTextureLocation, true);
//		glUniform1i(normalTextureLocation, normalTextureUnit);
//		normalTexture->useTexture(normalTextureUnit);
//	} else {
		glUniform1i(useNormalTextureLocation, false);
//	}

//	if (glowTexture && displayGlowTexture) {
//		glUniform1i(useGlowTextureLocation, true);
//		glUniform1i(glowTextureLocation, glowTextureUnit);
//		glowTexture->useTexture(glowTextureUnit);
//	} else {
		glUniform1i(useGlowTextureLocation, false);
//	}

//	if (specularTexture) {
//		glUniform1i(useSpecularTextureLocation, true);
//		glUniform1i(specularTextureLocation, specularTextureUnit);
//		specularTexture->useTexture(specularTextureUnit);
//	} else {
		glUniform1i(useSpecularTextureLocation, false);
//	}

	glActiveTexture(GL_TEXTURE0);
}

//PhongMaterial::PhongMaterial(const tinygltf::Material& material, const tinygltf::Model& model)
//		: PhongMaterial() {
//	try {
//		auto baseColorFactor = material.values.at("baseColorFactor");
//
//		auto color = baseColorFactor.ColorFactor();
//		diffuse = vec3(color[0], color[1], color[2]);
//	} catch (std::out_of_range& e) {
//		// swollow
//	}
//
//	try {
//		auto gltfTexture = material.values.at("baseColorTexture");
//
//		auto texID = gltfTexture.TextureIndex();
//		if (texID >= 0) {
//			diffuseTexture = Resources::loadTexture(model.textures[texID], model);
//		}
//	} catch (std::out_of_range& e) {
//		// swollow
//	}
//
//	try {
//		auto gltfTexture = material.additionalValues.at("specularTexture");
//
//		auto texID = gltfTexture.TextureIndex();
//		if (texID >= 0) {
//			specularTexture = Resources::loadTexture(model.textures[texID], model);
//		}
//	} catch (std::out_of_range& e) {
//		// swollow
//	}
//
//	try {
//		auto gltfTexture = material.additionalValues.at("normalTexture");
//
//		auto texID = gltfTexture.TextureIndex();
//		if (texID >= 0) {
//			normalTexture = Resources::loadTexture(model.textures[texID], model);
//		}
//	} catch (std::out_of_range& e) {
//		// swollow
//	}
//
//	try {
//		auto gltfTexture = material.additionalValues.at("emissiveTexture");
//
//		auto texID = gltfTexture.TextureIndex();
//		if (texID >= 0) {
//			glowTexture = Resources::loadTexture(model.textures[texID], model);
//		}
//	} catch (std::out_of_range& e) {
//		// swollow
//	}
//}
