#include <systems/render-engine/text/shader/TextShader.hpp>

using namespace NAISE::RenderCore;

TextShader::TextShader()
		: Shader("engine/resources/shaders/text/text.vert", "engine/resources/shaders/text/text.frag") {
}
