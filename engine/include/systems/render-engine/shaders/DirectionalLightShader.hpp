#pragma once

#include "LightShader.hpp"

namespace NAISE {
namespace Engine {

class DirectionalLightShader: public NAISE::RenderCore::LightShader {
public:
	DirectionalLightShader();

	void setShadowMapViewProjection(mat4 shadowMapViewProjection);

private:
	GLint depthViewProjectionLocation = -1;
};

}
}
