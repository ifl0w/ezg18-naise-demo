#pragma once

#include "Light.hpp"

namespace NAISE {
namespace Engine {

class PointLight :
		public LightComponent {
public:
	PointLight();

	PointLight(vec3 position);

	PointLight(vec3 position, vec3 color);

	/*
		default attenuation is 1, 0, 0
	*/
	void setAttenuation(float constant, float linear, float quadratic);

protected:
	bool cull() override;
private:
	float calculateLightVolumeRadius();
};

}
}