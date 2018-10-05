#include <systems/render-engine/materials/PBRMaterial.hpp>
#include <systems/render-engine/materials/shaders/PBRShader.hpp>
#include <Resources.hpp>

using namespace NAISE::RenderCore;

bool PBRMaterial::displayNormalTexture = true;
bool PBRMaterial::displayGlowTexture = true;

PBRMaterial::PBRMaterial(vec3 albedo, float metallic, float roughness)
		: albedo(albedo),
		  metallic(metallic),
		  roughness(roughness) {
	shader = Resources::getShader<PBRShader>();

	this->materialAlbedoLocation = uniformLocation(shader->shaderID, "material.albedo");
	this->materialRoughnessLocation = uniformLocation(shader->shaderID, "material.roughness");
	this->materialMetallicLocation = uniformLocation(shader->shaderID, "material.metallic");
	this->materialEmissionLocation = uniformLocation(shader->shaderID, "material.emission");

	this->albedoTextureLocation = uniformLocation(shader->shaderID, "albedoTexture");
	this->useAlbedoTextureLocation = uniformLocation(shader->shaderID, "useAlbedoTexture");

	this->roughnessTextureLocation = uniformLocation(shader->shaderID, "roughnessTexture");
	this->useRoughnessTextureLocation = uniformLocation(shader->shaderID, "useRoughnessTexture");

	this->metallicTextureLocation = uniformLocation(shader->shaderID, "metallicTexture");
	this->useMetallicTextureLocation = uniformLocation(shader->shaderID, "useMetallicTexture");

	this->normalTextureLocation = uniformLocation(shader->shaderID, "normalTexture");
	this->useNormalTextureLocation = uniformLocation(shader->shaderID, "useNormalTexture");

	this->emissionTextureLocation = uniformLocation(shader->shaderID, "emissionTexture");
	this->useEmissionTextureLocation = uniformLocation(shader->shaderID, "useEmissionTexture");

	this->useWatermeshAnimationLocation = uniformLocation(shader->shaderID, "useWatermeshAnimation");
}

void PBRMaterial::useMaterial() const {
	glUniform3fv(this->materialAlbedoLocation, 1, glm::value_ptr(albedo));
	glUniform1f(this->materialRoughnessLocation, roughness);
	glUniform1f(this->materialMetallicLocation, metallic);
	glUniform3fv(this->materialEmissionLocation, 1, glm::value_ptr(glow));
	glUniform1i(this->useWatermeshAnimationLocation, false);
//
//	if (diffuseTexture) {
//		glUniform1i(useColorTextureLocation, true);
//		glUniform1i(colorTextureLocation, colorTextureUnit);
//		diffuseTexture->useTexture(colorTextureUnit);
//	} else {
		glUniform1i(useAlbedoTextureLocation, false);
//	}

//	if (glowTexture && displayGlowTexture) {
//		glUniform1i(useGlowTextureLocation, true);
//		glUniform1i(glowTextureLocation, glowTextureUnit);
//		glowTexture->useTexture(glowTextureUnit);
//	} else {
		glUniform1i(useRoughnessTextureLocation, false);
//	}

//	if (specularTexture) {
//		glUniform1i(useSpecularTextureLocation, true);
//		glUniform1i(specularTextureLocation, specularTextureUnit);
//		specularTexture->useTexture(specularTextureUnit);
//	} else {
		glUniform1i(useMetallicTextureLocation, false);
//	}

//	if (normalTexture && displayNormalTexture) {
//		glUniform1i(useNormalTextureLocation, true);
//		glUniform1i(normalTextureLocation, normalTextureUnit);
//		normalTexture->useTexture(normalTextureUnit);
//	} else {
	glUniform1i(useNormalTextureLocation, false);
//	}

//	if (normalTexture && displayNormalTexture) {
//		glUniform1i(useNormalTextureLocation, true);
//		glUniform1i(normalTextureLocation, normalTextureUnit);
//		normalTexture->useTexture(normalTextureUnit);
//	} else {
	glUniform1i(useEmissionTextureLocation, false);
//	}

	glActiveTexture(GL_TEXTURE0);
}

PBRMaterial::PBRMaterial(const tinygltf::Material& material, const tinygltf::Model& model)
		: PBRMaterial() {
	try {
		auto baseColorFactor = material.values.at("baseColorFactor");

		auto color = baseColorFactor.ColorFactor();
		albedo = vec3(color[0], color[1], color[2]);
	} catch (std::out_of_range& e) {
		// swollow
	}

	try {
		auto gltfTexture = material.values.at("baseColorTexture");

		auto texID = gltfTexture.TextureIndex();
		if (texID >= 0) {
			albedoTexture = Resources::loadTexture(model.textures[texID], model);
		}
	} catch (std::out_of_range& e) {
		// swollow
	}

	try {
		auto gltfTexture = material.additionalValues.at("specularTexture");

		auto texID = gltfTexture.TextureIndex();
		if (texID >= 0) {
			metallicTexture = Resources::loadTexture(model.textures[texID], model);
		}
	} catch (std::out_of_range& e) {
		// swollow
	}

	try {
		auto gltfTexture = material.additionalValues.at("normalTexture");

		auto texID = gltfTexture.TextureIndex();
		if (texID >= 0) {
			roughnessTexture = Resources::loadTexture(model.textures[texID], model);
		}
	} catch (std::out_of_range& e) {
		// swollow
	}

	try {
		auto gltfTexture = material.additionalValues.at("normalTexture");

		auto texID = gltfTexture.TextureIndex();
		if (texID >= 0) {
			normalTexture = Resources::loadTexture(model.textures[texID], model);
		}
	} catch (std::out_of_range& e) {
		// swollow
	}

	try {
		auto gltfTexture = material.additionalValues.at("emissiveTexture");

		auto texID = gltfTexture.TextureIndex();
		if (texID >= 0) {
			emissionTexture = Resources::loadTexture(model.textures[texID], model);
		}
	} catch (std::out_of_range& e) {
		// swollow
	}
}
