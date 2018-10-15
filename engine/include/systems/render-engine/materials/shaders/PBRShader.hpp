#pragma once

#include "Shader.hpp"

namespace NAISE {
namespace Engine {

class PBRShader :
		public Shader {
public:
	PBRShader(): Shader("engine/shaders/deferred-shading/pbr.vert", "engine/shaders/deferred-shading/pbr.frag") {};

};

}
}