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
	float roughness = 0.3;
	float metallic = 0.0;
	vec3 glow = vec3(0); // color (vec3(0) = now glow)

	std::shared_ptr<Texture> albedoTexture;
	std::shared_ptr<Texture> metallicRoughnessTexture;
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

	uint32_t albedoTextureUnit = 1;
	GLint albedoTextureLocation = -1;
	GLint useAlbedoTextureLocation = -1;

	uint32_t metallicRoughnessTextureUnit = 2;
	GLint metallicRoughnessTextureLocation = -1;
	GLint useMetallicRoughnessTextureLocation = -1;

	uint32_t normalTextureUnit = 3;
	GLint normalTextureLocation = -1;
	GLint useNormalTextureLocation = -1;

	uint32_t emissionTextureUnit = 4;
	GLint emissionTextureLocation = -1;
	GLint useEmissionTextureLocation = -1;

};

}
}
