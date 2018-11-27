#include <systems/render-engine/text/shader/TextShader.hpp>

using namespace NAISE::RenderCore;

TextShader::TextShader()
		: Shader("assets/shaders/text/text.vert", "assets/shaders/text/text.frag") {
}
