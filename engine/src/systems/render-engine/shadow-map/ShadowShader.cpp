#include <sstream>
#include <utility>

#include "ShadowShader.hpp"

#include "../../lights/Light.h"

ShadowShader::ShadowShader()
		: Shader("engine/shaders/shadows/shadow-map.vert",
				 "engine/shaders/shadows/shadow-map.frag") {
}

