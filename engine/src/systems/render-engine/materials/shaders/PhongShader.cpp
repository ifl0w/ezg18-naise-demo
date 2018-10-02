#include <systems/render-engine/materials/shaders/PhongShader.hpp>

using namespace NAISE::Engine;

PhongShader::PhongShader()
		: Shader("engine/shaders/deferred-shading/phong.vert", "engine/shaders/deferred-shading/phong.frag") {
}
