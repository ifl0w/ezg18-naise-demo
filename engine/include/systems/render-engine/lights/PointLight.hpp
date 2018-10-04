#pragma once

#include <systems/render-engine/lights/Light.hpp>

namespace NAISE {
namespace RenderCore {

class PointLight :
		public Light {
public:
	PointLight();

	PointLight(vec3 position);

	PointLight(vec3 position, vec3 color);

	/*
		default attenuation is 1, 0, 0
	*/
	void setAttenuation(float constant, float linear, float quadratic);

	float calculateLightVolumeRadius();

protected:
	bool cull() override;
};

}
}