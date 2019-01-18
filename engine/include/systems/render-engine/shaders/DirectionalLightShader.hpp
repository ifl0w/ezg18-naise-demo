#pragma once

#include "LightShader.hpp"

namespace NAISE {
namespace Engine {

class DirectionalLightShader: public NAISE::RenderCore::LightShader {
public:
	DirectionalLightShader();

	void setShadowMapViewProjection(int position, mat4 shadowMapViewProjection);
	void setCascadeEnd(int position, mat4 shadowMapViewProjection, double distance);

private:
	GLint depthViewProjectionLocation[CASCADE_COUNT] = {-1};
	GLint cascadeEndLocation[CASCADE_COUNT] = {-1};


};

}
}
