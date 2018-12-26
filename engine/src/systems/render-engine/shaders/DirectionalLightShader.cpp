#include <sstream>
#include <systems/render-engine/shaders/DirectionalLightShader.hpp>

using namespace NAISE::Engine;

DirectionalLightShader::DirectionalLightShader()
		: LightShader("engine/resources/shaders/deferred-shading/light-shaders/directional_light.vert",
					  "engine/resources/shaders/deferred-shading/light-shaders/directional_light.frag") {
	depthViewProjectionLocation[0] = uniformLocation(shaderID, "depthShadowProjection[0]");
	depthViewProjectionLocation[1] = uniformLocation(shaderID, "depthShadowProjection[1]");
	depthViewProjectionLocation[2] = uniformLocation(shaderID, "depthShadowProjection[2]");

	cascadeEndLocation[0] = uniformLocation(shaderID, "cascadeEnd[0]");
	cascadeEndLocation[1] = uniformLocation(shaderID, "cascadeEnd[1]");
	cascadeEndLocation[2] = uniformLocation(shaderID, "cascadeEnd[2]");
}

void DirectionalLightShader::setShadowMapViewProjection(int position, mat4 shadowMapViewProjection) {
	glUniformMatrix4fv(this->depthViewProjectionLocation[position], 1, false, glm::value_ptr(shadowMapViewProjection));
}

void DirectionalLightShader::setCascadeEnd(int position, mat4 shadowMapViewProjection, double distance) {
	glm::vec4 shadowClip = shadowMapViewProjection * vec4(0,0,-distance,1);
	glUniform1f(this->cascadeEndLocation[position], shadowClip.z / shadowClip.w);
}
