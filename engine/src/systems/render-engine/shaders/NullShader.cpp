#include <systems/render-engine/shaders/NullShader.hpp>

using namespace NAISE::Engine;

NullShader::NullShader()
		: Shader("engine/shaders/deferred-shading/light-shaders/light_volume.vert",
				 "engine/shaders/deferred-shading/light-shaders/null.frag") {
}
