#include <sstream>
#include <systems/render-engine/shaders/DirectionalLightShader.hpp>

using namespace NAISE::Engine;

DirectionalLightShader::DirectionalLightShader()
		: LightShader("engine/resources/shaders/deferred-shading/light-shaders/directional_light.vert",
					  "engine/resources/shaders/deferred-shading/light-shaders/directional_light.frag") {

	for (int i = 0; i < CASCADE_COUNT; ++i) {
		depthViewProjectionLocation[i] = uniformLocation(shaderID, "depthShadowProjection[" + std::to_string(i) + "]");
		cascadeEndLocation[i] = uniformLocation(shaderID, "cascadeEnd[" + std::to_string(i) + "]");

	}
}

void DirectionalLightShader::setShadowMapViewProjection(int position, mat4 shadowMapViewProjection) {
	glUniformMatrix4fv(this->depthViewProjectionLocation[position], 1, false, glm::value_ptr(shadowMapViewProjection));
}

void DirectionalLightShader::setCascadeEnd(int position, mat4 shadowMapViewProjection, double distance) {
	glm::vec4 shadowClip = shadowMapViewProjection * vec4(0,0,-distance,1);
	glUniform1f(this->cascadeEndLocation[position], shadowClip.z / shadowClip.w);
}
