#include <systems/render-engine/shaders/PointLightShader.hpp>

using namespace NAISE::Engine;

PointLightShader::PointLightShader()
		: LightShader("engine/resources/shaders/deferred-shading/light-shaders/light_volume.vert",
					  "engine/resources/shaders/deferred-shading/light-shaders/point_light.frag") {
	this->debugLightVolumesLocation = uniformLocation(shaderID, "debugLightVolumes");
}

void PointLightShader::setLightVolumeDebugging(bool value) {
	glUniform1i(debugLightVolumesLocation, value);
}
