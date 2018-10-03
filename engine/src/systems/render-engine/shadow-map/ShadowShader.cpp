#include <sstream>
#include <utility>

#include <systems/render-engine/shadow-map/ShadowShader.hpp>

using namespace NAISE::Engine;

ShadowShader::ShadowShader()
		: Shader("engine/shaders/shadows/shadow-map.vert",
				 "engine/shaders/shadows/shadow-map.frag") {
}

