#include <sstream>
#include <utility>

#include <systems/render-engine/shadow-map/ShadowShader.hpp>

using namespace NAISE::Engine;

ShadowShader::ShadowShader()
		: Shader("engine/resources/shaders/shadows/shadow-map.vert",
				 "engine/resources/shaders/shadows/shadow-map.frag") {
}

