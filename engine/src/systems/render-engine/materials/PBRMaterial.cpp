#include <systems/render-engine/materials/PBRMaterial.hpp>
#include <systems/render-engine/materials/shaders/PBRShader.hpp>
#include <Resources.hpp>

using namespace NAISE::RenderCore;

bool PBRMaterial::displayNormalTexture = true;
bool PBRMaterial::displayGlowTexture = true;
bool PBRMaterial::useSkyboxTexture = false;

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

	this->metallicRoughnessTextureLocation = uniformLocation(shader->shaderID, "metallicRoughnessTexture");
	this->useMetallicRoughnessTextureLocation = uniformLocation(shader->shaderID, "useMetallicRoughnessTexture");

	this->normalTextureLocation = uniformLocation(shader->shaderID, "normalTexture");
	this->useNormalTextureLocation = uniformLocation(shader->shaderID, "useNormalTexture");

	this->emissionTextureLocation = uniformLocation(shader->shaderID, "emissionTexture");
	this->useEmissionTextureLocation = uniformLocation(shader->shaderID, "useEmissionTexture");

	this->useWatermeshAnimationLocation = uniformLocation(shader->shaderID, "useWatermeshAnimation");

	this->skyboxTextureLocation = uniformLocation(shader->shaderID, "skyboxTexture");
	this->useSkyboxTextureLocation = uniformLocation(shader->shaderID, "useSkyboxTexture");
}

void PBRMaterial::useMaterial() const {
	glUniform3fv(this->materialAlbedoLocation, 1, glm::value_ptr(albedo));
	glUniform1f(this->materialRoughnessLocation, roughness);
	glUniform1f(this->materialMetallicLocation, metallic);
	glUniform3fv(this->materialEmissionLocation, 1, glm::value_ptr(glow));
	glUniform1i(this->useWatermeshAnimationLocation, false);

	if (albedoTexture) {
		glUniform1i(useAlbedoTextureLocation, true);
		glUniform1i(albedoTextureLocation, albedoTextureUnit);
		albedoTexture->useTexture(albedoTextureUnit);
	} else {
		glUniform1i(useAlbedoTextureLocation, false);
	}

	if (emissionTexture && displayGlowTexture) {
		glUniform1i(useEmissionTextureLocation, true);
		glUniform1i(emissionTextureLocation, emissionTextureUnit);
		emissionTexture->useTexture(emissionTextureUnit);
	} else {
		glUniform1i(useEmissionTextureLocation, false);
	}

	if (metallicRoughnessTexture) {
		glUniform1i(useMetallicRoughnessTextureLocation, true);
		glUniform1i(metallicRoughnessTextureLocation, metallicRoughnessTextureUnit);
		metallicRoughnessTexture->useTexture(metallicRoughnessTextureUnit);
	} else {
		glUniform1i(useMetallicRoughnessTextureLocation, false);
	}

	if (normalTexture && displayNormalTexture) {
		glUniform1i(useNormalTextureLocation, true);
		glUniform1i(normalTextureLocation, normalTextureUnit);
		normalTexture->useTexture(normalTextureUnit);
	} else {
		glUniform1i(useNormalTextureLocation, false);
	}

	if (skyboxTexture && useSkyboxTexture) {
		glUniform1i(useSkyboxTextureLocation, true);
		glUniform1i(skyboxTextureLocation, skyboxTextureUnit);
		skyboxTexture->useTexture(skyboxTextureUnit);
	} else {
		glUniform1i(useSkyboxTextureLocation, false);
	}

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
		auto factor = material.values.at("metallicFactor");

		metallic = static_cast<float>(factor.Factor());
	} catch (std::out_of_range& e) {
		// swollow
	}

	try {
		auto factor = material.values.at("roughnessFactor");

		roughness = static_cast<float>(factor.Factor());
	} catch (std::out_of_range& e) {
		// swollow
	}

	try {
		auto gltfTexture = material.values.at("metallicRoughnessTexture");

		auto texID = gltfTexture.TextureIndex();
		if (texID >= 0) {
			metallicRoughnessTexture = Resources::loadTexture(model.textures[texID], model);
		}
	} catch (std::out_of_range& e) {
		metallic = 1.0;
		roughness = 1.0;
	}

	try {
		auto factor = material.values.at("emissiveFactor");

		auto color = factor.ColorFactor();
		glow = vec3(color[0], color[1], color[2]);
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

	try {
		auto gltfTexture = material.additionalValues.at("normalTexture");

		auto texID = gltfTexture.TextureIndex();
		if (texID >= 0) {
			normalTexture = Resources::loadTexture(model.textures[texID], model);
		}
	} catch (std::out_of_range& e) {
		// swollow
	}

}
