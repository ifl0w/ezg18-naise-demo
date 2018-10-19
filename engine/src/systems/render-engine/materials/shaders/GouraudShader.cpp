#include <systems/render-engine/materials/shaders/GouraudShader.hpp>

using namespace NAISE::Engine;

GouraudShader::GouraudShader()
		: Shader("engine/resources/shaders/gouraud_vertex_shader.glsl", "engine/resources/shaders/gouraud_fragment_shader.glsl") {
}

