#pragma once

#include "Material.hpp"
#include "../textures/Texture.hpp"
#include "systems/render-engine/meshes/Mesh.hpp"

//using namespace NAISE::Engine::Texture;

namespace NAISE {
namespace RenderCore {

class PBRMaterial : public Material {
public:

	PBRMaterial(): PBRMaterial(vec3(0.8), 0, 0.5) { };
	PBRMaterial(vec3 albedo): PBRMaterial(albedo, 1, 1) { };
	PBRMaterial(vec3 albedo, float metallic, float roughness);
	PBRMaterial(const tinygltf::Material& material, const tinygltf::Model& model);
//	explicit PhongMaterial(vec3 diffuse);
//	PhongMaterial(vec3 diffuse, vec3 specular, vec3 ambient, float alpha);
//	PhongMaterial(vec3 color, float diffuseFactor, float specularFactor, float ambientFactor, float alpha);

	void useMaterial() const override;

	vec3 albedo;
	float roughness;
	float metallic;
	vec3 glow = vec3(0); // color (vec3(0) = now glow)

	std::shared_ptr<Texture> albedoTexture;
	std::shared_ptr<Texture> roughnessTexture;
	std::shared_ptr<Texture> metallicTexture;
	std::shared_ptr<Texture> normalTexture;
	std::shared_ptr<Texture> emissionTexture;

	/* Flags for enabeling/disabeling */
	static bool displayNormalTexture;
	static bool displayGlowTexture;

private:
	GLint materialAlbedoLocation = -1;
	GLint materialRoughnessLocation = -1;
	GLint materialMetallicLocation = -1;
	GLint materialEmissionLocation = -1;

	GLint useWatermeshAnimationLocation = -1;

	uint32_t albedoTextureUnit = 5;
	GLint albedoTextureLocation = -1;
	GLint useAlbedoTextureLocation = -1;

	uint32_t roughnessTextureUnit = 6;
	GLint roughnessTextureLocation = -1;
	GLint useRoughnessTextureLocation = -1;

	uint32_t metallicTextureUnit = 7;
	GLint metallicTextureLocation = -1;
	GLint useMetallicTextureLocation = -1;

	uint32_t normalTextureUnit = 8;
	GLint normalTextureLocation = -1;
	GLint useNormalTextureLocation = -1;

	uint32_t emissionTextureUnit = 9;
	GLint emissionTextureLocation = -1;
	GLint useEmissionTextureLocation = -1;

};

}
}
