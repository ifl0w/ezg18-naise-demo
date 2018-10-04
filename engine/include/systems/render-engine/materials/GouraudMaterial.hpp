#pragma once

#include "Material.hpp"

namespace NAISE {
namespace RenderCore {

class GouraudMaterial : public Material {
public:
	GouraudMaterial();

	GouraudMaterial(vec3 diffuse);

	GouraudMaterial(vec3 diffuse, vec3 specular, vec3 ambient, float alpha);

	GouraudMaterial(vec3 color,
					float diffuseFactor,
					float specularFactor,
					float ambientFactor,
					float alpha
	);

	void useMaterial() const override;

	vec3 diffuse; // kd
	vec3 specular; // ks
	vec3 ambient; // ka
	float shininess; // alpha but better name

//	std::shared_ptr<Texture> diffuseTexture;
//	std::shared_ptr<Texture> specularTexture;

private:
	GLint materialDiffLocation = -1;
	GLint materialSpecLocation = -1;
	GLint materialAmbiLocation = -1;
	GLint materialShinLocation = -1;

	uint32_t colorTextureUnit = 0;
	GLint colorTextureLocation = -1;
	GLint useColorTextureLocation = -1;

	uint32_t specularTextureUnit = 1;
	GLint specularTextureLocation = -1;
	GLint useSpecularTextureLocation = -1;
};

}
}
