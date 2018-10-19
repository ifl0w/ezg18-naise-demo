#include <sstream>
#include <systems/render-engine/shaders/DirectionalLightShader.hpp>

using namespace NAISE::Engine;

DirectionalLightShader::DirectionalLightShader()
		: LightShader("engine/resources/shaders/deferred-shading/light-shaders/directional_light.vert",
					  "engine/resources/shaders/deferred-shading/light-shaders/directional_light.frag") {
	depthViewProjectionLocation = uniformLocation(shaderID, "depthShadowProjection");
}

void DirectionalLightShader::setShadowMapViewProjection(mat4 shadowMapViewProjection) {
	glUniformMatrix4fv(this->depthViewProjectionLocation, 1, false, glm::value_ptr(shadowMapViewProjection));
}
