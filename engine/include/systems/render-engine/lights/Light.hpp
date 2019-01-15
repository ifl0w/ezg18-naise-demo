#pragma once

#include <systems/render-engine/frustum-culling/AABB.hpp>

#include <glm/glm.hpp>

#include <memory>

using namespace glm;

namespace NAISE {
namespace RenderCore {

/**
 * Light data matching the data in GLSL
 */
struct LightData {
  vec4 lightPosition;
  vec4 specular; // is
  vec4 diffuse; // id
  vec4 ambient; // ia
  vec4 direction;
  float attConstant;
  float attLinear;
  float attQuadratic;
  float coneAngle;
  float coneBlend;
  uint32_t directional;
  float __padding[2]; // padding to match §7.6.2.2 of the OpenGL 4.5 specification (https://khronos.org/registry/OpenGL/specs/gl/glspec45.core.pdf)
};

class Light {
public:
	Light();

	Light(vec3 position,
		  vec3 diffuse,
		  vec3 specular,
		  vec3 ambient,
		  float attConstant,
		  float attLinear,
		  float attQuadratic,
		  float coneAngle,
		  float coneBlend,
		  vec3 direction,
		  bool directional);

	LightData data;

	virtual mat4 getShadowMatrix() const { return mat4(1); };
	virtual mat4 getProjectionMatrix() const { return mat4(1); };
	virtual mat4 getProjectionMatrix(AABB aabb) const { return mat4(1); };

	/**
	 * Defines whether the light has to be added allways.
	 * Lights that are not flagged as required, may be excluded from rendering if a certain threshold is exeeded.
	 */
	bool required = true;
	
protected:
	virtual bool cull();
};

}
}