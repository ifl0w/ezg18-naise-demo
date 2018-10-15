#pragma once

//#include "../../utilities/Utils.h"

#include "LightShader.hpp"

namespace NAISE {
namespace Engine {

class PointLightShader: public LightShader {
public:
	PointLightShader();

	void setLightVolumeDebugging(bool value);
private:
	GLint debugLightVolumesLocation = -1;
};

}
}
