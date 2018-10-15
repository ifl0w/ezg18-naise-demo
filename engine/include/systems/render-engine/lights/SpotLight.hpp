#pragma once

#include "Light.hpp"

namespace NAISE {
namespace Engine {

class SpotLight :
		public LightComponent {
public:
	SpotLight();

	SpotLight(vec3 position, vec3 color, vec3 direction, float angular, float blend);

	/*
	default attenuation is 1, 0, 0
	*/
	void setAttenuation(float constant, float linear, float quadratic);
};

}
}