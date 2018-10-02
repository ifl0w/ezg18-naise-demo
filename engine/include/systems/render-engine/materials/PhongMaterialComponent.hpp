#pragma once

#include "Material.hpp"
#include "systems/render-engine/meshes/Mesh.hpp"

//using namespace NAISE::Engine::Texture;

namespace NAISE {
namespace Engine {

class PhongMaterialComponent : public Component,
							   public Material {
public:
	PhongMaterialComponent();
//	PhongMaterial(const tinygltf::Material& material, const tinygltf::Model& model);
	explicit PhongMaterialComponent(vec3 diffuse);
	PhongMaterialComponent(vec3 diffuse, vec3 specular, vec3 ambient, float alpha);
	PhongMaterialComponent(vec3 color, float diffuseFactor, float specularFactor, float ambientFactor, float alpha);

	void useMaterial() const override;

	vec3 diffuse; // kd
	vec3 specular; // ks
	vec3 ambient; // ka
	float shininess; // alpha but better name
	vec3 glow = vec3(0); // color (vec3(0) = now glow)

//	std::shared_ptr<Texture> diffuseTexture;
//	std::shared_ptr<Texture> specularTexture;
//	std::shared_ptr<Texture> normalTexture;
//	std::shared_ptr<Texture> glowTexture;
	/* Flags for enabeling/disabeling */
	static bool displayNormalTexture;
	static bool displayGlowTexture;

private:
	GLint materialDiffLocation = -1;
	GLint materialSpecLocation = -1;
	GLint materialAmbiLocation = -1;
	GLint materialShinLocation = -1;
	GLint materialGlowLocation = -1;

	GLint useWatermeshAnimationLocation = -1;

	uint32_t colorTextureUnit = 0;
	GLint colorTextureLocation = -1;
	GLint useColorTextureLocation = -1;

	uint32_t specularTextureUnit = 6;
	GLint specularTextureLocation = -1;
	GLint useSpecularTextureLocation = -1;

	uint32_t normalTextureUnit = 2;
	GLint normalTextureLocation = -1;
	GLint useNormalTextureLocation = -1;

	uint32_t glowTextureUnit = 5;
	GLint glowTextureLocation = -1;
	GLint useGlowTextureLocation = -1;

};

}
}
