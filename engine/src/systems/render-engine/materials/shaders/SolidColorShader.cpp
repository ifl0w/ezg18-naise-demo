#include <systems/render-engine/materials/shaders/SolidColorShader.hpp>

using namespace NAISE::Engine;

SolidColorShader::SolidColorShader()
		: Shader("engine/shaders/simple_vertex_shader.glsl", "engine/shaders/color_fragment_shader.glsl") {
}
