#pragma once

#include "Shader.hpp"

namespace NAISE {
namespace Engine {

class PBRShader :
		public Shader {
public:
	PBRShader(): Shader("engine/resources/shaders/deferred-shading/pbr.vert", "engine/resources/shaders/deferred-shading/pbr.frag") {};

};

}
}